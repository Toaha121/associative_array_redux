#ifndef	__KEY_VALUE_TRIE_HEADER__
#define	__KEY_VALUE_TRIE_HEADER__

#include <stdio.h>

#include <aarray.h>

typedef struct KeyValueTrie KeyValueTrie;

/**
 ** PROTOTYPES
 **/

/** creation and deletion */
KeyValueTrie *trieCreateTrie();
void trieDeleteTrie(KeyValueTrie *trie);

/** iteration and printing */
void triePrint(FILE *fp, KeyValueTrie *);
int trieIterateAction(
		KeyValueTrie *trie,
		int (*userfunction)(AAKeyType key, size_t keylen, void *datavalue, void *userdata),
		void *userdata
	);

/** API access */
int trieInsertKey(KeyValueTrie *root,
		AAKeyType key, size_t keylength,
		void *value, int *cost);

void *trieLookupKey(KeyValueTrie *root, AAKeyType key, size_t keylength, int *cost);
void *trieDeleteKey(KeyValueTrie *root, AAKeyType key, size_t keylength, int *cost);


#endif
