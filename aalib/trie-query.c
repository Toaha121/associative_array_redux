#include <stdio.h>
#include <string.h> // for strcmp()
#include <ctype.h> // for isprint()
#include <stdlib.h> // for malloc()
#include <assert.h>

#include "trie_defs.h"


/** find a key within the trie */
void *trieLookupKey(KeyValueTrie *root, AAKeyType key, size_t keylength, int *cost)
{
	// TO DO: walk the trie to find the key, returning
	// a value if there is one after we have finished
	// our walk

	TrieNode *current = NULL;

    // Start from the root of the trie
    for (int i = 0; i < root->nSubtries; i++) {
        if (root->subtries[i]->letter == key[0]) {
            current = root->subtries[i];
            break;
        }
    }

    // If the first letter is not found, the key does not exist in the trie
    if (!current){
        return NULL;
    }

    // Traverse the trie following the key
    for (size_t i = 1; i < keylength; i++) {
        int found = 0;
        for (int j = 0; j < current->nSubtries; j++) {
            if (current->subtries[j]->letter == key[i]) {
                current = current->subtries[j];
                found = 1;
                if (cost) (*cost)++;
                break;
            }
        }
        // If the next letter is not found, the key does not exist in the trie
        if (!found){
            return NULL;
        } 
    }

    // If the end of the key is reached and the current node is marked as a key node, return its value
    if (current->isKeySoHasValue) {
        return current->value;
    }


	/** return null if the node doesn't have a value */
	return NULL;
}


