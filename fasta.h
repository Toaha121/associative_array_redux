#ifndef	__FASTA_RECORD_TOOLS_HEADER__
#define	__FASTA_RECORD_TOOLS_HEADER__

/**
 * Macros to define lengths for parsing
 */
#define	FASTA_MAX_ID_LEN 15
#define	FASTA_MAX_SEQUENCE_LINES 1024
#define	FASTA_RECOMMENDED_LINE_LENGTH 80
#define	FASTA_MAX_DESCRIPTION_LINE_LENGTH 1024

/**
 * Define our record type with a char based ID field
 */
typedef struct FASTArecord {
	char id[FASTA_MAX_ID_LEN+1];
	char *description;
	char *sequence;
} FASTArecord;

/** prototypes */
int  fastaReadRecord(FILE *ifp, FASTArecord *fRecord);
void fastaInitializeRecord(FASTArecord *fRecord);
FASTArecord * fastaAllocateRecord();
int  fastaPrintRecord(FILE *ofp, FASTArecord *fRecord);
void fastaClearRecord(FASTArecord *fRecord);
void fastaDeallocateRecord(FASTArecord *fRecord);

void clearFastaArray(FASTArecord *list, int maxRecords);
int loadFastaArray(FASTArecord *list, int maxRecords, char *filename);

#endif /* __FASTA_RECORD_TOOLS_HEADER__ */
