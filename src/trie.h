#ifndef _TRIE_H_
#define _TRIE_H_

#include "doublylinkedlist.h"


typedef struct {
	struct treeNode *child[52]; //0-25 + 26-51
	int cnt[52];
	int childNum;
}TreeNode;

typedef struct {
	TreeNode *root;
}Trie;

typedef LinkedList TextList;

static void initNode(TreeNode *node);

void initTire(Trie *tree);

void addStringToTrie(TreeNode *root, char *str);

int deleteStringInTrie(TreeNode *root, char *str);

TreeNode *searchString(TreeNode *root, char *str);

TextList *matchPrefix(TreeNode *root, char *str);

#endif
