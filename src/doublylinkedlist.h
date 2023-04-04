#ifndef _DOUBLY_LINKED_LIST_H_
#define _DOUBLY_LINKED_LIST_H_

// struct of a node in the linked list
typedef struct {
	void* datptr;
	struct listNode *prev;
	struct listNode *next;
} ListNode;

//pointer of the node
typedef ListNode* Listptr;

/*
	define a struct of a doubly linked list
	this is a warpped struct containing all variables
	used as the ONLY identification of a list
*/
typedef struct {
	Listptr head;
	Listptr tail;
	int listLen;
} LinkedList;

void initList(LinkedList* list);

void addNodeToTail(LinkedList* list, void* datptr);

void addNodeToHead(LinkedList* list, void* datptr);

Listptr kthNode(LinkedList* list, int index);

void modifyNode(Listptr node, void* datptr);
#endif
