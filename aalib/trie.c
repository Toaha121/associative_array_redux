#include <stdio.h>
#include <string.h> // for strcmp()
#include <ctype.h> // for isprint()
#include <stdlib.h> // for malloc()
#include <assert.h>

#include "trie_defs.h"


/** create the overall trie */
KeyValueTrie *
trieCreateTrie()
{
	int i;
    KeyValueTrie *root;

    root = (KeyValueTrie *) malloc(sizeof(KeyValueTrie));
    if (root == NULL) {
        // Handle the memory allocation failure
        return NULL;
    }

    // Assuming KeyValueTrie has a member named 'subtries' which is an array of TrieNode pointers
    root->subtries = (TrieNode **) malloc(256 * sizeof(TrieNode *));
    if (root->subtries == NULL) {
        // Handle the memory allocation failure
        free(root);
        return NULL;
    }

    for (i = 0; i < 256; i++) {
        root->subtries[i] = NULL;
    }
    root->nSubtries = 0;
    root->maxKeyLength = 0;
    return root;
}

/** recursive iterator helper function for deletion */
static void
trie_delete_helper(TrieNode *node)
{
	int i;

	if (node == NULL)	return;

	for (i = 0 ; i < node->nSubtries; i++) {
		trie_delete_helper(node->subtries[i]);
	}
	free(node);
}

/** delete the entire trie and all keys */
void
trieDeleteTrie(KeyValueTrie *trie)
{
	int i;

	for (i = 0 ; i < trie->nSubtries; i++) {
		trie_delete_helper(trie->subtries[i]);
	}
	free(trie);
}

/** create and initialize an empty node */
TrieNode *
trieCreateNode()
{
	TrieNode *newNode = (TrieNode *) malloc(sizeof(TrieNode));
	memset(newNode, 0, sizeof(TrieNode));
	return newNode;
}

/** clean up a single node */
void
trieDeleteNode(TrieNode *node)
{
	free(node);
}



#define	INDENT	4

/** recursive helper for printing */
static void
trie_print_sub_trie(FILE *fp, TrieNode *node, int depth)
{
	int i;

	for (i = 0; i < depth; i++) {
		fprintf(fp, "%*s", INDENT, "");
	}

	if (isprint(node->letter))
		fprintf(fp, "[%c]%c", node->letter,
				node->isKeySoHasValue ? '+' : ' ');
	else
		fprintf(fp, "[0x%02x]%c", node->letter,
				node->isKeySoHasValue ? '+' : ' ');
	while (node->nSubtries == 1) {
		node = node->subtries[0];
		depth++;
		if (isprint(node->letter))
			fprintf(fp, "[%c]%c", node->letter,
					node->isKeySoHasValue ? '+' : ' ');
		else
			fprintf(fp, "[0x%02x]%c", node->letter,
					node->isKeySoHasValue ? '+' : ' ');
	}
	fprintf(fp, "\n");
	for (i = 0; i < node->nSubtries; i++) {
		trie_print_sub_trie(fp, node->subtries[i], depth+1);
	}
}


/* print out the trie nicely, sideways */
void
triePrint(FILE *fp, KeyValueTrie *root)
{
	int i;

	if (root->nSubtries == 0) {
		fprintf(fp, "This trie is empty!\n");
		return;
	}

	for (i = 0; i < root->nSubtries; i++) {
		fprintf(fp, "%03d:\n", i);
		trie_print_sub_trie(fp, root->subtries[i], 1);
	}
}

