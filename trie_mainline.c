#include <stdio.h>
#include <string.h> /* for strlen(), strdup() */
#include <stdlib.h> /* for free() */
#include <unistd.h> /* for getopt() */
#include <ctype.h>  /* for isdigit() */
#include <time.h>   /* for clock() */
#include <errno.h>

#include "trie.h"
#include "data-reader.h"
#include "keyprint.h"

#define	LINE_MAX	128

/**
 * Load the trie of attribute value entries
 */
static int
loadKeyValueTrie(KeyValueTrie *trie, char *filename, int useIntKey)
{
	char linebuffer[LINE_MAX];
	char *strkey = NULL, *value = NULL;
	clock_t startTime, endTime;
	double timeTaken;
	int nEntries = 0;
	int intkey, cost = 0;
	FILE *fp = NULL;

	fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error: Failed to open input file '%s' : %s\n",
				filename, strerror(errno));
		return -1;
	}

	startTime = clock();
	while (readDataLine(fp, linebuffer, LINE_MAX, &strkey, &value) > 0) {
		if (useIntKey && isdigit(strkey[0])) {
			if (sscanf(strkey, "%d", &intkey) != 1) {
				fprintf(stderr, "Error: Failed extracting integer from '%s'\n", strkey);
				return -1;
			}
			if (trieInsertKey(trie,
						(AAKeyType) &intkey, sizeof(int),
						strdup(value), &cost) < 0) {
				fprintf(stderr, "Failed to add key '%d' to trie\n", intkey);
				return -1;
			}
		} else {

			if (trieInsertKey(trie,
						(AAKeyType) strkey, strlen(strkey),
						strdup(value), &cost) < 0) {
				fprintf(stderr, "Failed to add key '%s' to trie\n", strkey);
				return -1;
			}
		}
		nEntries++;
	}
	endTime = clock();

	timeTaken = ((double) (endTime - startTime)) / CLOCKS_PER_SEC;
	printf("Inserts took %lf seconds with reported cost %d\n",
			timeTaken, cost);


	fclose(fp);
	return nEntries;
}


/**
 * Query the trie with all the values in the given file
 */
static int
queryKeyValueTrie(KeyValueTrie *trie, char *filename, int useIntKey)
{
	char linebuffer[LINE_MAX];
	char *strkey = NULL, *value = NULL;
	clock_t startTime, endTime;
	double timeTaken;
	int intkey, cost = 0;
	FILE *fp = NULL;

	fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error: Failed to open query input file '%s' : %s\n",
				filename, strerror(errno));
		return -1;
	}

	startTime = clock();
	while (readPlainLine(fp, linebuffer, LINE_MAX, &strkey)) {
		if (useIntKey && isdigit(strkey[0])) {
			if (sscanf(strkey, "%d", &intkey) != 1) {
				fprintf(stderr, "Error: Failed extracting integer from '%s'\n", strkey);
				return -1;
			}

			value = trieLookupKey(trie,
					(AAKeyType) &intkey, sizeof(int), &cost);
			if (value == NULL) {
				printf("LOOKUP: key (%d) produced no value\n", intkey);
			} else {
				printf("LOOKUP: key (%d) produced value '%s'\n", intkey, value);
			}

		} else {
			value = trieLookupKey(trie,
					(AAKeyType) strkey, strlen(strkey), &cost);
			if (value == NULL) {
				printf("LOOKUP: key '%s' produced no value\n", strkey);
			} else {
				printf("LOOKUP: key '%s' produced value '%s'\n", strkey, value);
			}
		}
	}
	endTime = clock();

	timeTaken = ((double) (endTime - startTime)) / CLOCKS_PER_SEC;
	printf("Queries took %lf seconds with reported cost %d\n",
			timeTaken, cost);

	fclose(fp);
	return 1;
}

/**
 * Delete the selected values from the trie.  Note that we free the values
 * as otherwise they are memory leaks as we are managing the memory for
 * these values outside of the library
 */
static int
deleteFromKeyValueTrie(KeyValueTrie *trie, char *filename, int useIntKey)
{
	char linebuffer[LINE_MAX];
	char *strkey = NULL, *value = NULL;
	clock_t startTime, endTime;
	double timeTaken;
	int intkey, cost = 0;
	FILE *fp = NULL;

	fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error: Failed to open query input file '%s' : %s\n",
				filename, strerror(errno));
		return -1;
	}

	startTime = clock();
	while (readPlainLine(fp, linebuffer, LINE_MAX, &strkey)) {
		if (useIntKey && isdigit(strkey[0])) {
			if (sscanf(strkey, "%d", &intkey) != 1) {
				fprintf(stderr, "Error: Failed extracting integer from '%s'\n", strkey);
				return -1;
			}

			value = trieDeleteKey(trie,
					(AAKeyType) &intkey, sizeof(int), &cost);
			if (value == NULL) {
				printf("DELETE: key (%d) produced no value\n", intkey);
			} else {
				printf("DELETE: key (%d) produced value '%s'\n", intkey, value);
				free(value);
			}

		} else {
			value = trieDeleteKey(trie,
					(AAKeyType) strkey, strlen(strkey), &cost);
			if (value == NULL) {
				printf("DELETE: key '%s' produced no value\n", strkey);
			} else {
				printf("DELETE: key '%s' produced value '%s'\n", strkey, value);
				free(value);
			}
		}
	}
	endTime = clock();

	timeTaken = ((double) (endTime - startTime)) / CLOCKS_PER_SEC;
	printf("Deletions took %lf seconds with reported cost %d\n",
			timeTaken, cost);

	fclose(fp);
	return 1;
}


static int
printIteratorValue(AAKeyType key, size_t keylen, void *value, void *userdata)
{
	char keybuffer[LINE_MAX];
		// we will simply truncate keys at this length

	FILE *ofp = (FILE *) userdata;

	if (printableKey(keybuffer, LINE_MAX, key, keylen) < 0) {
		fprintf(stderr, "Error: key conversion failed!");
		return -1;
	}

	fprintf(ofp, "  Iterator Key: %s %p\n", keybuffer, value);

	return 0;
}

static int
deleteValue(AAKeyType key, size_t keylen, void *value, void *userdata)
{
	if (value != NULL)	free(value);
	return 0;
}

#define	DEFAULT_ARRAY_SIZE	100
#define OPTIONLEN	10

/** print out the help */
void usage(char *progname)
{
	fprintf(stderr, "%s [<OPTIONS>] <datafile> [ <datafile> ... ]\n", progname);
	fprintf(stderr, "\n");
	fprintf(stderr, "Creates a trie and loads it with values from\n");
	fprintf(stderr, "the data files given.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Options: \n");
	fprintf(stderr, "%-*s: Print this help.\n", OPTIONLEN, "-h");
	fprintf(stderr, "%-*s: If a key is made of digits, store it as an int.\n", OPTIONLEN, "-i");
	fprintf(stderr, "%-*s: Demonstrate the iterator by printing each key in turn\n", OPTIONLEN, "-I");
	fprintf(stderr, "%-*s: Output file to write to, default stdout.\n",
			OPTIONLEN, "-o <FILE>");
	fprintf(stderr, "%-*s: Print out the trie after processing.\n", OPTIONLEN, "-p");
	fprintf(stderr, "%-*s: Perform queries on all of the keys listed in <FILE> (one per line)\n",
			OPTIONLEN, "-q <FILE>");
	fprintf(stderr, "%-*s: Delete all of the keys listed in <FILE> (one per line)\n",
			OPTIONLEN, "-d <FILE>");
	fprintf(stderr, "\n");
	fprintf(stderr, "The order of the operations controlled by -d, -q and -p are: deletion first,\n");
	fprintf(stderr, "followed by any queries, and then finally printing (if indicated)\n");
	fprintf(stderr, "\n");
	exit (1);
}

/**
 * Program mainline -- loads all data from files listed into the hash.
 * Uses getopt(3) to parse arguments.
 */
int
main(int argc, char **argv)
{
	KeyValueTrie *trie;
	char *programname = NULL;
	FILE *ofp = stdout;
	int useIntKey = 0;
	int iterateContents = 0;
	int printContents = 0;
	char *queryfile = NULL, *deletefile = NULL;
	int i, c;


	/* save program name before calling getopt() */
	programname = argv[0];

	/** use getopt(3) to parse command line */
	while ((c = getopt(argc, argv, "hpiIo:q:d:")) != -1) {
		if (c == 'i') {
			useIntKey = 1;
		} else if (c == 'I') {
			iterateContents = 1;
		} else if (c == 'p') {
			printContents = 1;

		} else if (c == 'q') {
			queryfile = optarg;

		} else if (c == 'd') {
			deletefile = optarg;

		} else if (c == 'o') {
			ofp = fopen(optarg, "w");
			if (ofp == NULL) {
				fprintf(stderr,
						"Error: cannot open requested output file '%s' : %s\n",
						optarg, strerror(errno));
				usage(programname);
			}

		} else if (c == 'h') {
			usage(programname);
		}
	}

	/** update argc + argv to skip past flags */
	argc -= optind;
	argv += optind;

	if (argc < 1) {
		fprintf(stderr, "Error: No data files listed to load!\n");
		usage(programname);
	}

	/**
	 ** Allocate the trie and fail out if we cannot
	 **
	 ** None of the arguments required for the AssociativeArray
	 ** class are required here.
	 **/
	trie = trieCreateTrie();
	if (trie == NULL) {
		fprintf(stderr, "Error: cannot allocate trie - exitting\n");
		return -1;
	}


	/** getopt leaves us only "file" arguments left in argv */
	for (i = 0; i < argc; i++) {
		if (loadKeyValueTrie(trie, argv[i], useIntKey) < 0) {
			fprintf(stderr, "Error: failed loading from file '%s'\n",
					argv[i]);
			return -1;
		}
	}
	printf("Trie loaded\n");
	


	/** delete anything that we were asked to */
	if (deletefile != NULL) {
		deleteFromKeyValueTrie(trie, deletefile, useIntKey);
	}
	
	


	/** perform any queries we were asked to */
	if (queryfile != NULL) {
		queryKeyValueTrie(trie, queryfile, useIntKey);
	}
	
	

	/** iterate (printing each key) */
	if (iterateContents) {
		trieIterateAction(trie, printIteratorValue, stdout);
	}
	
	

	/* print out what we loaded */
	if (printContents) {
		triePrint(ofp, trie);
	}
	
	

	/* clean up before exit */
	trieIterateAction(trie, deleteValue, NULL);
	trieDeleteTrie(trie);
	

	/* exit with success if we get here */
	return 0;
}

