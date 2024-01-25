#include <stdio.h>
#include <string.h> // for strcmp()
#include <ctype.h> // for isprint()
#include <stdlib.h> // for malloc()
#include <assert.h>

#include "trie_defs.h"



/** compare a key with the letter in a node */
int trie_subtreeSearchComparator(const void *keyValue, const void *nodePtr)
{
	TrieNode **node = (TrieNode **) nodePtr;

	AAKeyType key = (AAKeyType) keyValue;
	return (key[0] - (*node)->letter);
}


/** create a whole chain for the rest of the key */
static TrieNode * trie_create_chain(AAKeyType key, size_t keylength, void *value, int *cost)
{
	TrieNode *current = NULL;

	// TO DO: create a new chain the the required key letters
	
	TrieNode *prevNode = NULL;

    //creating new node and initializing it's contents to zero
    for (size_t i = 0; i < keylength; i++) {
        TrieNode *newNode = trieCreateNode();
        newNode->letter = key[i];
        
        // when last letter in the key is reached
        if (i == keylength - 1) { 
            newNode->isKeySoHasValue = 1;
            newNode->value = value;
        }

        //initializing the previous node
        if (prevNode != NULL) {
            prevNode->subtries = malloc(sizeof(TrieNode*));
            prevNode->subtries[0] = newNode;
            prevNode->nSubtries = 1;
        } else {
            current = newNode; // Pointing to first node in the chain
        }

        //previous node points to new node
        prevNode = newNode;

        if (cost != NULL) {
            (*cost)++;
            }
    }


	return current;
}

/** add the given chain to the list of tries in the current node */
static int trie_add_chain(TrieNode ***subtreeList, int *nSubtries, AAKeyType key, TrieNode *newChain)
{
	// TO DO:  add the provided chain to the node; the first letter
	// of the key may be used to organize where within the set of
	// subtries you add this
	
    //resizing pointer to take more subtries
    *subtreeList = realloc(*subtreeList, sizeof(TrieNode*) * (*nSubtries + 1));
    
    // Checking memory allocation failure
    if (*subtreeList == NULL) {
        return -1; 
    }

    //adding chain
    (*subtreeList)[*nSubtries] = newChain;
    (*nSubtries)++;

   

	// TO DO: you probably want to replace this return statement
	// with your own code 
    
    return 0; 

}


/** link the provided key into the current chain */
static int trie_link_to_chain(TrieNode *current, AAKeyType key, size_t keylength, void *value, int *cost)
{
	// TO DO: add the remaining portions of the key
	// into this chain, forming a new branch if and when
	// they stop matching existing letters within the subtries

    //checking if current key matches or not
	 while (keylength > 0 && current->nSubtries > 0) {
        int found = 0;
        for (int i = 0; i < current->nSubtries; i++) {
            if (current->subtries[i]->letter == key[0]) {
                current = current->subtries[i];
                key++;
                keylength--;
                found = 1;
                break;
            }
        }
        if (!found) {
            break;
        }
    }
 
 
    // creating a new chain for the remaining key
    if (keylength > 0) {
        TrieNode *newChain = trie_create_chain(key, keylength, value, cost);
        current->subtries = realloc(current->subtries, sizeof(TrieNode*) * (current->nSubtries + 1));
        current->subtries[current->nSubtries] = newChain;
        current->nSubtries = current->nSubtries + 1;
    } else {
        current->isKeySoHasValue = 1;
        current->value = value;
    }


	// TO DO: you probably want to replace this return statement
	// with your own code

	 return 0; 
}


int
trieInsertKey(KeyValueTrie *root, AAKeyType key, size_t keylength, void *value, int *cost)
{ 
	/** keep the max key length in order to keep a buffer for interation */
	if (root->maxKeyLength < keylength)
		root->maxKeyLength = keylength;

	if (root->nSubtries == 0) {
		root->subtries[root->nSubtries++] = trie_create_chain(
				key, keylength, value, cost);
		return 0;
	}

	
	// TO DO: find the subtrie with the leading letter of the
	// key, and insert the new key into the correct subtrie
	// chain based on that letter

	for (int i = 0; i < root->nSubtries; i++) {
        if (root->subtries[i]->letter == key[0]) {
            return trie_link_to_chain(root->subtries[i], key + 1, keylength - 1, value, cost);
        }
    }
     // If first letter not found, create a new chain
    TrieNode *newChain = trie_create_chain(key, keylength, value, cost);
    trie_add_chain(&(root->subtries), &(root->nSubtries), key, newChain);


	// TO DO: you probably want to replace this return statement
	// with your own code

	return 0;

}



