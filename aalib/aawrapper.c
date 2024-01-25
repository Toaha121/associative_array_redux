#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include "trie_defs.h"

/**
 * Create an associative array using a trie.
 *
 * All the values needed for the hash table are simply ignored.
 */
AssociativeArray *
aaCreateAssociativeArray(
		size_t size,
		char *probingStrategy,
		char *hashPrimary,
		char *hashSecondary
	)
{
	AssociativeArray *newAA;

	newAA = (AssociativeArray *) malloc(sizeof(AssociativeArray));
	memset(newAA, 0, sizeof(AssociativeArray));

	newAA->trie = trieCreateTrie();
	return newAA;
}

/**
 * deallocate all the memory in the store -- the keys (which we allocated),
 * and the store itself.
 * The user * code is responsible for managing the memory for the values
 */
void
aaDeleteAssociativeArray(AssociativeArray *aarray)
{
	trieDeleteTrie(aarray->trie);
	free(aarray);
}

/**
 * Add another key and data value to the table, provided there is room.
 *
 *  @param  key  a string value used for searching later
 *  @param  value a data value associated with the key
 *  @return      the location the data is placed within the hash table,
 *				 or a negative number if no place can be found
 */
int
aaInsert(AssociativeArray *aarray,
		AAKeyType key, size_t keylen,
		void *value
	)
{
	return trieInsertKey(aarray->trie,
			key, keylen,
			value, &aarray->insertCost);
}


/**
 * Locates the KeyDataPair associated with the given key, if
 * present in the table.
 *
 *  @param  key  the key to search for
 *  @return      the KeyDataPair containing the key, if the key
 *				 was present in the table, or NULL, if it was not
 *  @see         KeyDataPair
 */
void *aaLookup(AssociativeArray *aarray, AAKeyType key, size_t keylen)
{
	return trieLookupKey(aarray->trie, key, keylen, &aarray->searchCost);
}


/**
 * Locates the KeyDataPair associated with the given key, if
 * present in the table.
 *
 *  @param  key  the key to search for
 *  @return      the index of the KeyDataPair containing the key,
 *				 if the key was present in the table, or (-1),
 *				 if no key was found
 *  @see         KeyDataPair
 */
void *aaDelete(AssociativeArray *aarray, AAKeyType key, size_t keylen)
{
	return trieDeleteKey(aarray->trie, key, keylen, &aarray->deleteCost);
}

/** iterate over the array, calling the user function on each valid value */
int aaIterateAction(
		AssociativeArray *aarray,
		int (*userfunction)(AAKeyType key, size_t keylen, void *datavalue, void *userdata),
		void *userdata
	)
{
	return trieIterateAction(aarray->trie, userfunction, userdata);
}

/**
 * Print out the entire aarray contents
 */
void aaPrintContents(FILE *fp, AssociativeArray *aarray, char * tag)
{
	triePrint(fp, aarray->trie);
}


/**
 * Print out a short summary
 */
void aaPrintSummary(FILE *fp, AssociativeArray *aarray)
{
	fprintf(fp, "Associative array contains %d entries\n",
			aarray->nEntries);
	fprintf(fp, "Costs accrued while processing keys:\n");
	fprintf(fp, "  Insertion : %d\n", aarray->insertCost);
	fprintf(fp, "  Search    : %d\n", aarray->searchCost);
	fprintf(fp, "  Deletion  : %d\n", aarray->deleteCost);
}
