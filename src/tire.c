#include <string.h>
#include "tire.h"
#include "codeparser.h"

static void initNode(TreeNode *node){
	node->childNum = 0;
	int i=0;
	for(i=0; i<26; i++) node->child[i] = NULL;
}

void initTire(Tire *tree){
	tree->root = (TreeNode *)malloc(sizeof(TreeNode));
	initNode(tree->root);
}

void addStringToTire(TreeNode *root, char *str){
	if(root == NULL || str == NULL) return;
	if(*str == '\0') return;
	
	char ch = *str;
	int index = 0;
	
	if(ch >= 'a' && ch <= 'z'){
		index = ch - 'a';
	}else{
		index = 26 + ch - 'A';
	}
	
	if(root->child[index] == NULL){
		TreeNode *newNode = (TreeNode *)malloc(sizeof(TreeNode));
		initNode(newNode);
		root->child[index] = newNode;
		root->childNum++;
		addStringToTire(newNode, str+1);
	}else{
		addStringToTire(root->child[index], str+1);
	}
}

static int charToIndex(char ch){
	if(ch >= 'a' && ch <= 'z'){
		return ch - 'a';
	}else{
		return 26 + ch - 'A';
	}	
}

static char indexToChar(int index){
	if(index >= 0 && index <= 25){
		return 'a' + index;
	}else{
		return 'A' + index - 26;
	}
}

TreeNode *searchString(TreeNode *root, char *str){
	if(root == NULL || str == NULL) return NULL;
	if(*str == '\0') return;
	
	char ch = *str;
	int index = 0;
	
	index = charToIndex(ch);
	
	if(root->child[index] == NULL){
		return NULL;
	}else{
		str++;
		if(*str == '\0'){
			return root;
		}else{
			return searchString(root->child[index], str);
		}
	}
}

static void getAllString(TextList *textList, TreeNode *root, int addIndex){
	if(root->childNum == 0) return;
	int i=0;
	for(i=0; i<52; i++){
		if(root->child[i] == NULL) continue;
		char *str = (char *)malloc(sizeof(char)*MAX_WORD_SIZE);
		int len = 0;
		*str = '\0';
		if(addIndex != 0){
			char *preStr = kthNode(textList, addIndex)->datptr;
			strcpy(str, preStr);
		}
		len = strlen(str);
		str[len] = indexToChar(i);
		str[len+1] = '\0';
		addNodeToTail(textList, str);
		getAllString(textList, root->child[i], textList->listLen);
	}
	if(addIndex != 0) deleteNode(textList, addIndex);
}

TextList *matchPrefix(TreeNode *root, char *str){
	TreeNode *startNode = searchString(root, str);
	
	if(startNode->childNum == 0) return NULL;
	
	TextList *textList = (TextList *)malloc(sizeof(TextList));
	initList(textList);
	
	getAllString(textList, startNode, 0);
	
	return textList;
}
