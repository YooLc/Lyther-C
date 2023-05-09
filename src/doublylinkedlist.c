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

void deleteNode(LinkedList* list, int index){
    if(index > list->listLen || index <= 0) return;
    
    Listptr nowNode = kthNode(list, index);
    list->listLen--;
    
    if(nowNode->prev == NULL){
        list->head = nowNode->next;
    }else{
        Listptr prevNode = nowNode->prev;
        prevNode->next = nowNode->next;
    }
    
    if(nowNode->next == NULL){
        list->tail = nowNode->prev;
    }else{
        Listptr nextNode = nowNode->next;
        nextNode->prev = nowNode->prev;
    }
    
    free(nowNode);
}

void deleteNodeByPtr(LinkedList* list, Listptr nowNode){
    list->listLen--;
//return ;
    if(nowNode->prev == NULL){
        list->head = nowNode->next;
    }else{
        Listptr prevNode = nowNode->prev;
        prevNode->next = nowNode->next;
    }
    
    if(nowNode->next == NULL){
        list->tail = nowNode->prev;
    }else{
        Listptr nextNode = nowNode->next;
        nextNode->prev = nowNode->prev;
    }
    
    free(nowNode);
    if(list->listLen == 0) list->head = list->tail = NULL;
}

void addNode(LinkedList* list, int index, void* datptr) {
    printf("Called addNode %d %d\n", index, list->listLen);
    if(index <= 0 || index > list->listLen + 1) return;
    
    if(index == 1) {
        addNodeToHead(list, datptr);
        return;
    }
    
    if(index == list->listLen + 1) {
        addNodeToTail(list, datptr);
        return;
    }
    
    list->listLen++;
    
    Listptr lastNode = kthNode(list, index - 1);
    Listptr nextNode = kthNode(list, index);
    Listptr newNode = (Listptr)malloc(sizeof(ListNode));

    newNode->datptr = datptr;
    newNode->next = lastNode->next;
    newNode->prev = lastNode;
    nextNode->prev = newNode;
    lastNode->next = newNode;
    
}

void deleteNode(LinkedList* list, int index){
	if(index > list->listLen || index <= 0) return;
	
	Listptr nowNode = kthNode(list, index);
	list->listLen--;
	
	if(nowNode->prev == NULL){
		list->head = nowNode->next;
	}else{
		Listptr prevNode = nowNode->prev;
		prevNode->next = nowNode->next;
	}
	
	if(nowNode->next == NULL){
		list->tail = nowNode->prev;
	}else{
		Listptr nextNode = nowNode->next;
		nextNode->prev = nowNode->prev;
	}
	
	free(nowNode);
}

void deleteNodeByPtr(LinkedList* list, Listptr nowNode){
	list->listLen--;
//return ;
	if(nowNode->prev == NULL){
		list->head = nowNode->next;
	}else{
		Listptr prevNode = nowNode->prev;
		prevNode->next = nowNode->next;
	}
	
	if(nowNode->next == NULL){
		list->tail = nowNode->prev;
	}else{
		Listptr nextNode = nowNode->next;
		nextNode->prev = nowNode->prev;
	}
	
	free(nowNode);
	if(list->listLen == 0) list->head = list->tail = NULL;
}

void addNode(LinkedList* list, int index, void* datptr){
	
	if(index <= 0 || index > list->listLen + 1) return;
	
	if(index == 1){
		addNodeToHead(list, datptr);
		return;
	}
	
	if(index == list->listLen + 1){
		addNodeToTail(list, datptr);
		return;
	}
	
	list->listLen++;
	
	Listptr lastNode = kthNode(list, index-1);
	Listptr nextNode = kthNode(list, index);
	Listptr newNode = (Listptr)malloc(sizeof(ListNode));

	newNode->datptr = datptr;
	newNode->next = lastNode->next;
	newNode->prev = lastNode;
	nextNode->prev = newNode;
	lastNode->next = newNode;
	
}
