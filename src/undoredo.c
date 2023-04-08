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

void Redo(UndoRedo *ur);

//when there is a new IO event, then clear the old redo
void clearRedo(UndoRedo *ur);

void addTrace(UndoRedo *ur, TraceType type, int rows, int cols, int rowt, int colt, char *content){
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
