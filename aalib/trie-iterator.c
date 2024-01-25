#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include "trie_defs.h"


/**
 * Iterate a single chain, calling the user function every time
 * we come to the end of a key
 */
static int
trie_iterate_chain(TrieNode *curnode, AAKeyType keybuffer, int keybufferpos,
		int (*userfunction)(AAKeyType key, size_t keylen, void *datavalue, void *userdata),
		void *userdata
	)
{
	int i, nLongerKeysOnChain = 0, nTotalKeys = 0;

	keybuffer[keybufferpos] = curnode->letter;

	if (curnode->isKeySoHasValue) {
		nTotalKeys++;
		keybuffer[keybufferpos+1] = '\0';
		if ((*userfunction)(keybuffer, keybufferpos+1,
				curnode->value, userdata) < 0) {
			return -1;
		}
	}

	for (i = 0; i < curnode->nSubtries; i++) {
		nLongerKeysOnChain = trie_iterate_chain(curnode->subtries[i],
				keybuffer, keybufferpos + 1,
				userfunction, userdata);
		if (nLongerKeysOnChain < 0)	return -1;
		nTotalKeys += nLongerKeysOnChain;
	}

	return nTotalKeys;
}

/**
 * Iterate over the array, calling the user function whenever we find
 * a valid value, as this ends a key
 */
int trieIterateAction(
		KeyValueTrie *trie,
		int (*userfunction)(AAKeyType key, size_t keylen, void *datavalue, void *userdata),
		void *userdata
	)
{
	/**
	 ** Walk each of the trie chains in turn, loading up the buffer
	 ** allocated here each time with the new key
	 **/
	AAKeyType buffer;
	int i, nKeys = 0, thisNkeys;

	/** buffer large enough for key and termination */
	buffer = (AAKeyType) malloc(trie->maxKeyLength + 1);

	for (i = 0 ; i < trie->nSubtries; i++) {
		thisNkeys = trie_iterate_chain(trie->subtries[i],
				buffer, 0, userfunction, userdata);
		if (thisNkeys < 0) {
			free(buffer);
			return -1;
		}
		nKeys += thisNkeys;
	}

	free(buffer);
	return nKeys;
}

