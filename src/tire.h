#ifndef _TIRE_H_
#define _TIRE_H_

#include "doublylinkedlist.h"


typedef struct {
	struct treeNode *child[52]; //0-25 + 26-51
	int cnt[52];
	int childNum;
}TreeNode;

typedef struct {
	TreeNode *root;
}Tire;

typedef LinkedList TextList;

static void initNode(TreeNode *node);

void initTire(Tire *tree);

void addStringToTire(TreeNode *root, char *str);

int deleteStringInTire(TreeNode *root, char *str);

TreeNode *searchString(TreeNode *root, char *str);

TextList *matchPrefix(TreeNode *root, char *str);

#endif
