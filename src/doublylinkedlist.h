#ifndef _DOUBLY_LINKED_LIST_H_
#define _DOUBLY_LINKED_LIST_H_

// struct of a node in the linked list
typedef struct ListNode{
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
typedef struct LinkedList{
    Listptr head;
    Listptr tail;
    int listLen;
} LinkedList;

void initList(LinkedList* list);

void addNodeToTail(LinkedList* list, void* datptr);

void addNodeToHead(LinkedList* list, void* datptr);

Listptr kthNode(LinkedList* list, int index);

int getNodeIndex(LinkedList* list, Listptr node);
	
void modifyNode(Listptr node, void* datptr);

void deleteNode(LinkedList* list, int index);

void deleteNodeByPtr(LinkedList* list, Listptr nowNode);

void addNode(LinkedList* list, int index, void* datptr);
#endif
