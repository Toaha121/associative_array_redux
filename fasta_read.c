#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#include "fasta.h"

/**
 * Extract the ID string from within the description
 */
static int
fastaExtractIDfromDescription(
			char *idbuffer,
			int maxIdLen,
			const char *fastaIDline
		)
{
	char *barLocation_1 = NULL, *barLocation_2 = NULL;
	int extractedLength;

	// find the | | locations
	if ((barLocation_1 = index(fastaIDline, '|')) == NULL)	return -1;
	if ((barLocation_2 = index(barLocation_1 + 1, '|')) == NULL) return -1;

	// figure out how many bytes between the bars and clip
	// for maximum length
	extractedLength = (barLocation_2) - (barLocation_1 + 1);
	if (extractedLength > maxIdLen)
		extractedLength = maxIdLen;

	// if nothing is left, return 0
	if (extractedLength == 0) {
		idbuffer[0] = '\0';
		return 0;
	}

	// copy in the string and terminate it
	memcpy(idbuffer, barLocation_1 + 1, extractedLength);
	idbuffer[extractedLength] = '\0';

	return extractedLength;
}

/**
 * Read in a FASTA record, allocating memory for the fields.
 */
int
fastaReadRecord(FILE *ifp, FASTArecord *fRecord)
{
	/**
	 * A line is "recommended" to be no more than 80 characters,
	 * and the longest sequence is likely no more than 10 lines
	 * of sequence.  The "description" portion is potentially
	 * longer than 80 characters, so we ensure that the relatively
	 * large buffer allocated here is sufficient for that length
	 * as the first action within this function.
	 *
	 * We can allocate a fairly large buffer here without undue
	 * concern that we are wasting memory as this buffer will be
	 * returned to the system as a local variable when this function
	 * returns. */
	char linebuffer[FASTA_MAX_SEQUENCE_LINES
			* FASTA_RECOMMENDED_LINE_LENGTH];
	char *fgetstatus;
	int curLoadIndex = 0, nLinesRead = 0;
	int bytesRemain, curBytesRead;

	/** if our assumption about the first line length is too large
	 * to fit into the allocated buffer, panic
	 */
	assert(FASTA_MAX_SEQUENCE_LINES * FASTA_RECOMMENDED_LINE_LENGTH
			> FASTA_MAX_DESCRIPTION_LINE_LENGTH);

	/** fill linebuffer with zeros */
	bzero(linebuffer, FASTA_MAX_SEQUENCE_LINES
			* FASTA_RECOMMENDED_LINE_LENGTH);

	linebuffer[0] = fgetc(ifp);
	if (linebuffer[0] <= 0) {
		return 0;
	}


	/**
	 * Handle the description.
	 *
	 * Read the rest of the line at the beginning of the loop
	 * adding the result into the current buffer
	 */
	curLoadIndex = 1;
	fgetstatus = fgets(&linebuffer[curLoadIndex],
			FASTA_MAX_DESCRIPTION_LINE_LENGTH, ifp);
	if (fgetstatus == NULL) {
		fprintf(stderr, "Error: FASTA parser encountered EOF"
				" during partial description line\n");
		printf("RETURNING from %d\n", __LINE__);
		return -1;
	}

	/* check if we have overflow */
	if (linebuffer[strlen(linebuffer) - 1] != '\n') {
		fprintf(stderr, "Error: FASTA parser read description"
				" line greater than %d characters\n",
				FASTA_MAX_DESCRIPTION_LINE_LENGTH);
		fprintf(stderr, "desc[%s] %lu '%c'\n", linebuffer, strlen(linebuffer),
				linebuffer[strlen(linebuffer)-1]);
		printf("RETURNING from %d\n", __LINE__);
		return -1;
	}
	nLinesRead++;
	fRecord->description = strdup(linebuffer);
	if (fastaExtractIDfromDescription(fRecord->id,
					FASTA_MAX_ID_LEN, linebuffer) < 0) {
		fprintf(stderr, "Error: FASTA parser failed extracting"
				" ID from line:\n");
		fprintf(stderr, "	 : '%s'\n", linebuffer);
		return -1;
	}

	/** handle the sequence */
	curLoadIndex = 0;
	bytesRemain = (FASTA_MAX_SEQUENCE_LINES
			* FASTA_RECOMMENDED_LINE_LENGTH) - 2;
	linebuffer[curLoadIndex] = fgetc(ifp);
	if (linebuffer[curLoadIndex] <= 0) {
		fprintf(stderr, "Error: FASTA parser encountered"
					" unexpected end of file before sequence data\n");
		free(fRecord->description);
		printf("RETURNING from %d\n", __LINE__);
		return -1;
	}

	/** collate all of the portions of the sequence */
	while (linebuffer[curLoadIndex] > 0 && linebuffer[curLoadIndex] != '>') {
		fgetstatus = fgets(&linebuffer[curLoadIndex], bytesRemain, ifp);
		if (fgetstatus == NULL) {
			fprintf(stderr, "Error: FASTA parser encountered"
					" unexpected end of file\n");
			free(fRecord->description);
		printf("RETURNING from %d\n", __LINE__);
			return -1;
		}
		nLinesRead++;
		curBytesRead = strlen(&linebuffer[curLoadIndex]);
		if (curBytesRead >= 80) {
			fprintf(stderr,
					"Warning: FASTA parser read sequence of length (%d);",
					curBytesRead);
			fprintf(stderr,
					" longer than 80 character recommendation!\n");
			fprintf(stderr, "	   : [%s]\n", &linebuffer[curLoadIndex]);
		}

		if (linebuffer[curLoadIndex + curBytesRead - 1] != '\n') {
			fprintf(stderr, "Error: FASTA parser sequence"
					" line overflows buff\n");
		printf("RETURNING from %d\n", __LINE__);
			return -1;
		}

		/* Now check what the first character of the next line is,
		 * and if it is a new record, push it back */
		bytesRemain -= curBytesRead;
		curLoadIndex += curBytesRead - 1;
		linebuffer[curLoadIndex] = fgetc(ifp);
		if (linebuffer[curLoadIndex] == '>') {
			ungetc('>', ifp);
			linebuffer[curLoadIndex] = 0;
		}
	}

	/** save the sequence */
	fRecord->sequence = strdup(linebuffer);

	return nLinesRead;
}

/**
 * print out a FASTA record
 */
int
fastaPrintRecord(FILE *ofp, FASTArecord *fRecord)
{
	fprintf(ofp, "FASTA Record:\n");
	fprintf(ofp, "ID   [%s]\n", fRecord->id);
	fprintf(ofp, "DESC [%s]\n", fRecord->description);
	fprintf(ofp, "SEQ  [%s]\n", fRecord->sequence);

	return 0;
}

/**
 * Initialize a FASTA record.
 * Useful when working with tables of records.
 */
void
fastaInitializeRecord(FASTArecord *fRecord)
{
	fRecord->description = NULL;
	fRecord->sequence = NULL;
	fRecord->id[0] = '\0';
}

/**
 * Allocate and initialize a new FASTA record
 */
FASTArecord *
fastaAllocateRecord()
{
	FASTArecord *fRecord = NULL;
	
	fRecord = (FASTArecord *) malloc(sizeof(FASTArecord));

	fastaInitializeRecord(fRecord);

	return fRecord;
}

/**
 * Clear the record but do not free the fRecord pointer.
 * Useful when working with tables of records.
 */
void
fastaClearRecord(FASTArecord *fRecord)
{
	if (fRecord->description != NULL) {
		free(fRecord->description);
		fRecord->description = NULL;
	}
	if (fRecord->sequence != NULL) {
		free(fRecord->sequence);
		fRecord->sequence = NULL;
	}
	fRecord->id[0] = '\0';
}

/**
 * Deallocate by clearing and then freeing the record itself
 */
void
fastaDeallocateRecord(FASTArecord *fRecord)
{
	fastaClearRecord(fRecord);
	free(fRecord);
}

/**
 * Load a set of records into a given array
 */
int
loadFastaArray(FASTArecord *list, int maxRecords, char *filename)
{
	int recordNumber = 0, status;
	int keepReading = 1;
	FILE *fp;

	fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "Failure opening %s : %s\n",
				filename, strerror(errno));
		return -1;
	}

	do {
		// print a '.' every 10,000 records so
		// we know something is happening
		if ((recordNumber % 10000) == 0) {
			printf(".");
			fflush(stdout);
		}

		status = fastaReadRecord(fp, &list[recordNumber]);
		if (status <= 0) {
			keepReading = 0;
		} else {
			/* record read successfully, so increment counter */
			recordNumber++;
		}

	} while ( (keepReading) && (recordNumber < maxRecords));

	if (status < 0) {
		fprintf(stderr, "Error: failure record %d in '%s'\n",
					recordNumber, filename);
		return -1;
	}

	printf(" %d FASTA records loaded\n", recordNumber);

	fclose(fp);

	return recordNumber;
}

/**
 * Clear a set of records loaded into a given array
 */
void
clearFastaArray(FASTArecord *list, int maxRecords)
{
	int i;

	for (i = 0; i < maxRecords; i++) {
		fastaClearRecord(&list[i]);
	}
}

