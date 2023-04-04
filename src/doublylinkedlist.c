#include "doublylinkedlist.h"
#include <stdio.h>
#include <stdlib.h>

void initList(LinkedList* list) {
	list->head = list->tail = NULL;
	list->listLen = 0;
}

void addNodeToTail(LinkedList* list, void* datptr) {
	Listptr newNode = (Listptr)malloc(sizeof(ListNode));
	newNode->datptr = datptr;
	
	if(list->listLen == 0){
		newNode->next = newNode->prev = NULL;
		list->head = list->tail = newNode;
		list->listLen++;
		return;
	}
	
	newNode->prev = list->tail;
	newNode->next = NULL;
	list->tail->next = newNode;
	list->tail = newNode;
	list->listLen++;
}

void addNodeToHead(LinkedList* list, void* datptr) {
	Listptr newNode = (Listptr)malloc(sizeof(ListNode));
	newNode->datptr = datptr;
	
	if(list->listLen == 0){
		
		newNode->next = newNode->prev = NULL;
		list->head = list->tail = newNode;
		list->listLen++;
		return;
	}
	
	newNode->next = list->head;
	newNode->prev = NULL;
	list->head->prev = newNode;
	list->head = newNode;
	list->listLen++;
}

Listptr kthNode(LinkedList* list, int index) {
	if(index > list->listLen || index <= 0) return NULL;
	
	int nowIndex = 1;
	Listptr nowptr = list->head;
	while(nowIndex < index){
		nowptr = nowptr->next;
		nowIndex++;
	}
	return nowptr;
}

void modifyNode(Listptr node, void* datptr) {
	node->datptr = datptr;
}
