#include "undoredo.h"
#include "doublylinkedlist.h"
#include "codeparser.h"

#include <string.h>

void Undo(UndoRedo *ur){
	if(ur->undoRedoList.listLen == 0 || ur->nowNode == NULL) return;
	
	Trace *trace = ur->nowNode->datptr;
	int rows, cols, rowt, colt;
	rows = trace->rows;
	cols = trace->cols;
	rowt = trace->rowt;
	colt = trace->colt;
	if(trace->type == ADD){
		deleteString(ur->passage, rows, cols, rowt, colt);
	}else if(trace->type == DELETE){
		addString(ur->passage, trace->content, rows, cols);
	}
	ur->nowNode = ur->nowNode->prev;
}

void Redo(UndoRedo *ur){
	if(ur->undoRedoList.listLen == 0) return;
	
	if(ur->nowNode == NULL){    //If it is before the head of the list
		ur->nowNode = ur->undoRedoList.head;
	}else{
		ur->nowNode = ur->nowNode->next;
	}
	
	if(ur->nowNode == NULL) return;
	
	Trace *trace = ur->nowNode->datptr;
	int rows, cols, rowt, colt;
	rows = trace->rows;
	cols = trace->cols;
	rowt = trace->rowt;
	colt = trace->colt;
	if(trace->type == ADD){
		addString(ur->passage, trace->content, rows, cols);
	}else if(trace->type == DELETE){
		deleteString(ur->passage, rows, cols, rowt, colt);
	}
}

static void clearRedo(UndoRedo *ur){
	if(ur->undoRedoList.listLen == 0) return;
	
	if(ur->nowNode == NULL){    //If it is before the head of the list
		while(ur->undoRedoList.head != NULL){
			deleteNodeByPtr(&(ur->undoRedoList), ur->undoRedoList.head);
		}
		return;
	}
	
	while(ur->nowNode->next != NULL){
		deleteNodeByPtr(&(ur->undoRedoList), ur->nowNode);
	}
}

void addTrace(UndoRedo *ur, TraceType type, int rows, int cols, int rowt, int colt, char *content){
	clearRedo(ur);

	Trace *trace = NEW(Trace);
	trace->rows = rows;
	trace->cols = cols;
	trace->rowt = rowt;
	trace->colt = colt;
	trace->type = type;
	strcpy(trace->content, content);

	addNodeToTail(&(ur->undoRedoList), trace);
	ur->nowNode = kthNode(&(ur->undoRedoList), ur->undoRedoList.listLen);
}

void initUndoRedoList(UndoRedo *ur, Passage *passage){
	ur->passage = passage;
	initList(&(ur->undoRedoList));
	ur->nowNode = NULL;
}
