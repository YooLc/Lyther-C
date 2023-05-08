#ifndef _TEXTAREA_H_
#define _TEXTAREA_H_

#include <stdbool.h>
#include "doublylinkedlist.h"
#include "linkedlist.h"
#include "codeparser.h"
#include "undoredo.h"
#include "style.h"

#define MAX_FILE_COUNT 20
#define LINE_INDEX_WIDTH .45
#define SCROLL_DIST (fontHeight*3)

/*
    Struct: Editor Form
    Single form containing single .c file.
    (x, y) denoted left-bottom point, while (x + w, y + h) is right-top point
*/
typedef struct {
    int style;
    bool visible, inSelectionMode;
    double x, y, w, h;
    PosRC caretPos, realCaretPos, renderPos, selectLeftPos, selectRightPos;
    UndoRedo *urStack;
    Passage *passage;
} EditorForm;

/*
    Struct: Editor
    Overall struct of an editor, containing several(probably) forms.
    curSelect(int): Number of working form, 1-based
    drawLock(bool): Every updated event should change status of drawLock
                    to avoid calling Display() when things are being updated
    updated(bool): Display() shall check, if there's no update, then do nothing
                   this means we need to separate the caret display to another function
*/
typedef struct {
    int fileCount, curSelect;
    double menuHeight, barHeight;
    bool drawLock, updated;
    char *filePath[MAX_FILE_COUNT];
    EditorForm *forms[MAX_FILE_COUNT];
} Editor;

typedef enum {
    UP, DOWN, LEFT, RIGHT
} CaretAction;

/*
    Function: initEditor
    Not well implemented.
*/
void initEditor(Editor* editor);
static void drawEditorMenu(Editor* editor);
static void drawEditorBar(Editor* editor);
static void drawEditorForm(EditorForm* form);
static void drawEditorSelection(EditorForm* from);
static void drawSymbolMatch(EditorForm *form);
static void drawCodeLine(EditorForm* form, Line* line, double x, double y, double w, double h);
static void drawToken(Token* token, double x, double y, double w, double h);
static void drawTokenBox(Token* token, double x, double y, double w, double h);
void drawMessageBar();

void addCodeToEditor(Editor* editor, FILE* fp, char* filePath);
static void drawCaret(EditorForm* form);
static void moveCaret(EditorForm *form, CaretAction action, char* curLine, char* preLine);
void moveCaretByMouse(Editor* editor, int x, int y, int button, int event);
void moveCaretByKey(Editor* editor, int key, int event);
void addChar(char ch);
#endif
