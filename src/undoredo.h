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

/*
	Struct of a UndoRedo abstract
	Creat a Instance of this struct for every passage
*/
typedef struct{
	Passage *passage;
	LinkedList undoRedoList;
	Listptr nowNode;
}UndoRedo;

//Call this function when there is an Undo event
void Undo(UndoRedo *ur);

//Call this function when there is an Redo event
void Redo(UndoRedo *ur);

/*
	When there is a new IO event, add this trace in the undoRedoList
	This function will automatically delete all the nodes after this new node
*/
void addTrace(UndoRedo *ur, TraceType type, int rows, int cols, int rowt, int colt, char *content);

//Internal function
static void clearRedo(UndoRedo *ur);

//Call this function immediately after instantiate a UndoRedo abstract
void initUndoRedoList(UndoRedo *ur, Passage *passage);

#endif
