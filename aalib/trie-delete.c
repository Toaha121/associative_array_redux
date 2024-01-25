#include <stdio.h>
#include <string.h> // for strcmp()
#include <ctype.h> // for isprint()
#include <stdlib.h> // for malloc()
#include <assert.h>

#include "trie_defs.h"


/** recursively roll up the chain */
static int walk_chain_to_delete(void **value, TrieNode *curSearchNode, AAKeyType key, size_t keylength, int *cost)
{
	// TO DO: remove nodes for deleted key
  
  
   // Reached end of key
   if (keylength == 0 || curSearchNode == NULL) {
        return 0;
    }

    // Search for the next node in the chain that matches the current key character.
    for (int i = 0; i < curSearchNode->nSubtries; i++) {
        if (curSearchNode->subtries[i]->letter == key[0]) {
            if (keylength == 1 && curSearchNode->subtries[i]->isKeySoHasValue) {
                *value = curSearchNode->subtries[i]->value;
                //curSearchNode->subtries[i]->isKeySoHasValue = 0;
                //curSearchNode->subtries[i]->value = NULL;
               
               
               return 1;
            }
            (*cost)++;

            // Recurse for the next character in the key.
            return walk_chain_to_delete(value, curSearchNode->subtries[i], key + 1, keylength - 1, cost);
        }
    }

    // Current character not found in any subtries.
    return 0;
}
	

/** delete a key from the trie */
void *trieDeleteKey(KeyValueTrie *root, AAKeyType key, size_t keylength, int *cost)
{
	void *valueFromDeletedKey = NULL;


	if (root->nSubtries == 0) {
		return NULL;
	}

	/**
	 ** TO DO: search for the right subchain and delete the key from it
	 **/

	
    // Iterate over the top-level nodes to find the starting node that matches the key's first character.
    for (int i = 0; i < root->nSubtries; i++) {
        if (root->subtries[i]->letter == key[0]) {
            if (walk_chain_to_delete(&valueFromDeletedKey, root->subtries[i], key + 1, keylength - 1, cost)) {
                break;  // If the key is found and deleted, break out of the loop.
            }
        }
    }

    return valueFromDeletedKey;
}



