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
	char content[MAX_LINE_SIZE];
}Trace;

typedef struct{
	Passage *passage;
	LinkedList undoRedoList;
	Listptr nowNode;
}UndoRedo;

void Undo(UndoRedo *ur);

void Redo(UndoRedo *ur);

//when there is a new IO event, add this trace in the undoRedoList
void addTrace(UndoRedo *ur, TraceType type, int rows, int cols, int rowt, int colt, char *content);

void initUndoRedoList(UndoRedo *ur, Passage *passage);

#endif
