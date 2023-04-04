#ifndef _BI_LINKED_LIST_H_
#define _BI_LINKED_LIST_H_

// struct of a node in the linked list
typedef struct listNode{
	void* datptr;
	struct listNode *next;
	struct listNode *last;
}listNode;

//pointer of the node
typedef listNode* listptr;

/*
	define a struct of a double linked list
	this is a warpped struct containing all variables
	used as the ONLY identification of a list
*/
typedef struct{
	listptr head;
	listptr tail;
	int listLen;
}linkedList;

void initList(linkedList* list);

void addNodeToTail(linkedList* list, void* datptr);

void addNodeToHead(linkedList* list, void* datptr);

listptr kthNode(linkedList* list, int index);

void modifyNode(listptr node, void* datptr);
#endif

