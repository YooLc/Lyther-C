#include "undoredo.h"
#include "doublylinkedlist.h"
#include "codeparser.h"

#include <string.h>

PosRC Undo(UndoRedo *ur)
{
    PosRC posRC = {-1, -1};

    if (ur->undoRedoList.listLen == 0 || ur->nowNode == NULL) return posRC;

    Trace *trace = ur->nowNode->datptr;
    printf("UNDO STRING %s\n", trace->content);
    int rows, cols, rowt, colt;
    rows = trace->rows;
    cols = trace->cols;
    rowt = trace->rowt;
    colt = trace->colt;
    printf("UNDO RANGE %d %d %d %d\n", rows, cols, rowt, colt);

    if (trace->type == ADD) {
        deleteString(ur->passage, rows, cols, rowt, colt);
        posRC.r = rows;
        posRC.c = cols - 1;
    } else if (trace->type == DELE)
        posRC = addString(ur->passage, trace->content, rows, cols);

    ur->nowNode = ur->nowNode->prev;
    printf("NEW POS AFTER UNDO %d %d\n", posRC.r, posRC.c);
    return posRC;
}

PosRC Redo(UndoRedo *ur)
{
    PosRC posRC = {-1, -1};

    if (ur->undoRedoList.listLen == 0) return posRC;

    if (ur->nowNode == NULL) //If it is before the head of the list
        ur->nowNode = ur->undoRedoList.head;
    else {
        if (ur->nowNode->next == NULL) return posRC;

        ur->nowNode = ur->nowNode->next;
    }

    //if(ur->nowNode == NULL) return posRC;
    Trace *trace = ur->nowNode->datptr;
    int rows, cols, rowt, colt;
    rows = trace->rows;
    cols = trace->cols;
    rowt = trace->rowt;
    colt = trace->colt;

    if (trace->type == ADD)
        posRC = addString(ur->passage, trace->content, rows, cols);
    else if (trace->type == DELE) {
        deleteString(ur->passage, rows, cols, rowt, colt);
        posRC.r = rows;
        posRC.c = cols - 1;
    }

    printf("NEW POS AFTER REDO %d %d\n", posRC.r, posRC.c);
    return posRC;
}

static void clearRedo(UndoRedo *ur)
{
    if (ur->undoRedoList.listLen == 0) return;

    if (ur->nowNode == NULL) { //If it is before the head of the list
        while (ur->undoRedoList.head != NULL)
            deleteNodeByPtr(&(ur->undoRedoList), ur->undoRedoList.head);

        return;
    }

    while (ur->nowNode->next != NULL)
        deleteNodeByPtr(&(ur->undoRedoList), ur->nowNode->next);
}

void addTrace(UndoRedo *ur, TraceType type, int rows, int cols, int rowt, int colt, char *content)
{
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

void initUndoRedoList(UndoRedo *ur, Passage *passage)
{
    ur->passage = passage;
    initList(&(ur->undoRedoList));
    ur->nowNode = NULL;
}
