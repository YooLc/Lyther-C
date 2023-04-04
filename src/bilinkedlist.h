#ifndef _BI_LINKED_LIST_H_
#define _BI_LINKED_LIST_H_

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

// struct of a node in the linked list
typedef struct{
	void* datptr;
	listNode *next;
	listNode *last;
}listNode;

//pointer of the node
typedef listNode* listptr;

void initList(linkedList* list);

void addNodeToTail(linkedList* list, void* datptr);

void addNodeToHead(linkedList* list, void* datptr);

#endif

