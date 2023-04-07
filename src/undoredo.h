#ifndef _UNDO_REDO_H_
#define _UNDO_REDO_H_

#include "doublylinkedlist.h"
#include "codeparser.h"

typedef enum{
	ADD,
	DELETE
}TraceType;

typedef struct{
	TraceType type;
	int rows, cols, rowt, colt; //interval (closed)
}Trace;

typedef struct{
	Passage *passage;
	Linkedlist undoRedoList;
	Listptr nowNode;
	int length;
}UndoRedoList;

void Undo(UndoRedoList *list);

void Redo(UndoRedoList *list);

//when there is a new IO event, then clear the old redo
void clearRedo(UndoRedoList *list);

#endif
