#include "bilinkedlist.h"

void initList(linkedList* list){
	list->head = list->tail = NULL;
}

void addNodeToTail(linkedList* list, void* datptr){
	listptr newNode = (listptr)malloc(sizeof(listNode));
	newNode->datptr = datptr;
	
	if(list->listLen == 0){
		
		newNode->next = newNode->last = NULL;
		head = tail = newNode;
		listLen++;
		return;
	}
	
	newNode->last = tail;
	newNode->next = NULL;
	tail = newNode;
	listLen++;
}

void addNodeToHead(linkedList* list, void* datptr){
	listptr newNode = (listptr)malloc(sizeof(listNode));
	newNode->datptr = datptr;
	
	if(list->listLen == 0){
		
		newNode->next = newNode->last = NULL;
		head = tail = newNode;
		listLen++;
		return;
	}
	
	newNode->next = head;
	newNode->last = NULL;
	head = newNode;
	listLen++;
}
