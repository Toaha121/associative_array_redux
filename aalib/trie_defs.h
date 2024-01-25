#ifndef	__TRIE_TOOLS_HEADER__
#define	__TRIE_TOOLS_HEADER__

#include <stdio.h>

#include <trie.h>

typedef char TrieLetter;

typedef struct TrieNode {
	struct TrieNode **subtries;
	TrieLetter letter;
	int nSubtries;
	int isKeySoHasValue;
	void *value;
} TrieNode;

typedef struct KeyValueTrie {
	struct TrieNode **subtries;
	int nSubtries;
	int maxKeyLength;
} KeyValueTrie;

struct AssociativeArray {
	KeyValueTrie *trie;
	int nEntries;
	int searchCost;
	int insertCost;
	int deleteCost;
};


/**
 ** PROTOTYPES
 **/

/** creation and deletion */
TrieNode * trieCreateNode();
void trieDeleteNode(TrieNode *node);

/** utilities */
int trie_subtreeSearchComparator(const void *keyValue, const void *nodePtr);

#endif
