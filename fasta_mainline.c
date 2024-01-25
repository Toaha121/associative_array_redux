#include <stdio.h>
#include <string.h> /* for strlen(), strdup() */
#include <stdlib.h> /* for free() */
#include <unistd.h> /* for getopt() */
#include <ctype.h>  /* for isdigit() */
#include <time.h>   /* for clock() */
#include <errno.h>

#include "aarray.h"
#include "fasta.h"
#include "data-reader.h"

#define	LINE_MAX	128

/**
 * Load the associative array of attribute value entries
 */
static int
loadAssociativeArray(AssociativeArray *assocArray, char *filename)
{
	FASTArecord *fRecord = NULL;
	clock_t startTime, endTime;
	double timeTaken;
	int nEntries = 0;
	FILE *fp = NULL;

	fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error: Failed to open input file '%s' : %s\n",
				filename, strerror(errno));
		return -1;
	}

	fRecord = fastaAllocateRecord();

	startTime = clock();
	while (fastaReadRecord(fp, fRecord) > 0) {
		if (aaInsert(assocArray,
						(AAKeyType) fRecord->id, strlen(fRecord->id),
						fRecord) < 0) {
			fprintf(stderr,
				"Failed to add FASTA record with key '%s' to associative array\n",
				fRecord->id);
			return -1;
		}
		nEntries++;
		fRecord = fastaAllocateRecord();
	}
	endTime = clock();

	timeTaken = ((double) (endTime - startTime)) / CLOCKS_PER_SEC;
	printf("Inserts took %lf seconds\n", timeTaken);

	/** the last record didn't get used */
	fastaDeallocateRecord(fRecord);

	fclose(fp);
	return nEntries;
}

/**
 * Query the associative array with all the values in the given file
 */
static int
queryAssociativeArray(AssociativeArray *assocArray, char *filename)
{
	char linebuffer[LINE_MAX];
	clock_t startTime, endTime;
	double timeTaken;
	char *strkey = NULL;
	FASTArecord *value = NULL;
	FILE *fp = NULL;

	fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr,
				"Error: Failed to open query input file '%s' : %s\n",
				filename, strerror(errno));
		return -1;
	}

	startTime = clock();
	while (readPlainLine(fp, linebuffer, LINE_MAX, &strkey)) {
		value = (FASTArecord *) aaLookup(assocArray,
				(AAKeyType) strkey, strlen(strkey));
		if (value == NULL) {
			printf("LOOKUP: key '%s' produced no value\n", strkey);
		} else {
			printf("LOOKUP: key '%s' produced record:\n", strkey);
			fastaPrintRecord(stdout, value);
		}
	}
	endTime = clock();

	timeTaken = ((double) (endTime - startTime)) / CLOCKS_PER_SEC;
	printf("Queries took %lf seconds\n", timeTaken);

	fclose(fp);
	return 1;
}

/**
 * Delete the selected values from the associative array.  Note that we free the values
 * as otherwise they are memory leaks as we are managing the memory for
 * these values outside of the library
 */
static int
deleteFromAssociativeArray(AssociativeArray *assocArray, char *filename)
{
	char linebuffer[LINE_MAX];
	char *strkey = NULL;
	FASTArecord *value = NULL;
	clock_t startTime, endTime;
	double timeTaken;
	FILE *fp = NULL;

	fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error: Failed to open query input file '%s' : %s\n",
				filename, strerror(errno));
		return -1;
	}

	startTime = clock();
	while (readPlainLine(fp, linebuffer, LINE_MAX, &strkey)) {
		value = (FASTArecord *) aaDelete(assocArray,
				(AAKeyType) strkey, strlen(strkey));
		if (value == NULL) {
			printf("DELETE: key '%s' produced no value\n", strkey);
		} else {
			printf("DELETE: successfully deleted record with key '%s' \n",
					strkey);
			fastaDeallocateRecord(value);
		}
	}
	endTime = clock();

	timeTaken = ((double) (endTime - startTime)) / CLOCKS_PER_SEC;
	printf("Deletions took %lf seconds\n", timeTaken);

	fclose(fp);
	return 1;
}


static int
deleteValue(AAKeyType key, size_t keylen, void *value, void *userdata)
{
	if (value != NULL)	fastaDeallocateRecord(value);
	return 0;
}

#define	DEFAULT_ARRAY_SIZE	100
#define OPTIONLEN	10

/** print out the help */
void usage(char *progname)
{
	fprintf(stderr, "%s [<OPTIONS>] <datafile> [ <datafile> ... ]\n", progname);
	fprintf(stderr, "\n");
	fprintf(stderr, "Creates an associative array and loads it with FASTA records\n");
	fprintf(stderr, "from the data files given.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Options: \n");
	fprintf(stderr, "%-*s: Print this help.\n", OPTIONLEN, "-h");
	fprintf(stderr, "%-*s: Output file to write to, default stdout.\n",
			OPTIONLEN, "-o <FILE>");
	fprintf(stderr, "%-*s: Size of table used internally, default %d.\n",
			OPTIONLEN, "-n <SIZE>", DEFAULT_ARRAY_SIZE);
	fprintf(stderr, "%-*s: Hash using the given algorithm.  Choices are \"sum\", \"length\",\n",
			OPTIONLEN, "-H <ALG>");
	fprintf(stderr, "%-*s: or your own algorithm.\n", OPTIONLEN, "");
	fprintf(stderr, "%-*s: Probe using the given algorithm.  Choices are \"linear\", \"quadratic\",\n",
			OPTIONLEN, "-P <ALG>");
	fprintf(stderr, "%-*s: or \"doublehash\".\n", OPTIONLEN, "");
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
	char *programname = NULL;
	FILE *ofp = stdout;
	int arraySize = DEFAULT_ARRAY_SIZE;
	int printContents = 0;
	char *queryfile = NULL, *deletefile = NULL;
	int i, c;

	AssociativeArray *assocArray;
	char *hash1 = "sum", *hash2 = "len", *probe = "lin";

	/* save program name before calling getopt() */
	programname = argv[0];

	/** use getopt(3) to parse command line */
	while ((c = getopt(argc, argv, "hpn:H:2:P:o:q:d:")) != -1) {
		if (c == 'p') {
			printContents = 1;

		} else if (c == 'n') {
			if (sscanf(optarg, "%d", &arraySize) != 1) {
				fprintf(stderr,
						"Error: cannot parse array"
						" size requested from '%s'\n",
						optarg);
				usage(programname);
			}

		} else if (c == 'H') {
		} else if (c == 'H') {
			hash1 = optarg;

		} else if (c == '2') {
			hash2 = optarg;

		} else if (c == 'P') {
			probe = optarg;

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

	/** allocate the associative array and fail out if we cannot */
	assocArray = aaCreateAssociativeArray(arraySize, probe, hash1, hash2);
	if (assocArray == NULL) {
		fprintf(stderr, "Error: cannot allocate associative array - exitting\n");
		return -1;
	}


	/** getopt leaves us only "file" arguments left in argv */
	for (i = 0; i < argc; i++) {
		if (loadAssociativeArray(assocArray, argv[i]) < 0) {
			fprintf(stderr, "Error: failed loading from file '%s'\n", argv[i]);
			return -1;
		}
	}
	printf("Associative array loaded\n");


	/** delete anything that we were asked to */
	if (deletefile != NULL) {
		deleteFromAssociativeArray(assocArray, deletefile);
	}

	/** perform any queries we were asked to */
	if (queryfile != NULL) {
		queryAssociativeArray(assocArray, queryfile);
	}

	/* print out what we loaded */
	if (printContents) {
		aaPrintContents(ofp, assocArray, "    ");
	}

	/* clean up before exit */
	aaIterateAction(assocArray, deleteValue, NULL);
	aaDeleteAssociativeArray(assocArray);

	/* exit with success if we get here */
	return 0;
}

