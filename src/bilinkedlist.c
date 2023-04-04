#include "bilinkedlist.h"
#include <stdio.h>

void initList(linkedList* list){
	list->head = list->tail = NULL;
}

void addNodeToTail(linkedList* list, void* datptr){
	listptr newNode = (listptr)malloc(sizeof(listNode));
	newNode->datptr = datptr;
	
	if(list->listLen == 0){
		
		newNode->next = newNode->last = NULL;
		list->head = list->tail = newNode;
		list->listLen++;
		return;
	}
	
	newNode->last = list->tail;
	newNode->next = NULL;
	list->tail = newNode;
	list->listLen++;
}

void addNodeToHead(linkedList* list, void* datptr){
	listptr newNode = (listptr)malloc(sizeof(listNode));
	newNode->datptr = datptr;
	
	if(list->listLen == 0){
		
		newNode->next = newNode->last = NULL;
		list->head = list->tail = newNode;
		list->listLen++;
		return;
	}
	
	newNode->next = list->head;
	newNode->last = NULL;
	list->head = newNode;
	list->listLen++;
}

listptr kthNode(linkedList* list, int index){
	if(index > list->listLen || index <= 0) return NULL;
	
	int nowIndex = 1;
	listptr nowptr = list->head;
	while(nowIndex < index){
		nowptr = nowptr->next;
		nowIndex++;
	}
	return nowptr;
}

void modifyNode(listptr node, void* datptr){
	node->datptr = datptr;
}
