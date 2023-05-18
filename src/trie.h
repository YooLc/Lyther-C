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

/*
	function initTire(Trie *tree);
	Call this when instantiate a new Trie
*/
void initTrie(Trie *tree);

/*
	function addStringToTrie(TreeNode *root, char *str);
	add <str> into tree with the root <root>
*/
void addStringToTrie(TreeNode *root, char *str);

/*
	function deleteStringInTrie(TreeNode *root, char *str);
	delete <str> in tree with the root <root>
	return value no needs to be considered
*/
int deleteStringInTrie(TreeNode *root, char *str);

/*
	function searchString(TreeNode *root, char *str);
	search <str> in tree with the root of <root>
	return the pointer to the endpoint
*/
TreeNode *searchString(TreeNode *root, char *str);

/*
	function matchPrefix(TreeNode *root, char *str);
	match string with perfix <str>
	return a LinkedList of matched string
	If failed, return NULL.
*/
TextList *matchPrefix(TreeNode *root, char *str);

void printAllString(TreeNode *root);

#endif
