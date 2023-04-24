#ifndef _TEXTAREA_H_
#define _TEXTAREA_H_

#include <stdbool.h>
#include "doublylinkedlist.h"
#include "linkedlist.h"
#include "codeparser.h"
#include "undoredo.h"
#include "style.h"

#define MAX_FILE_COUNT 20

/*
    Struct: Editor Form
    Single form containing single .c file.
    (x, y) denoted left-bottom point, while (x + w, y + h) is right-top point
*/
typedef struct {
    int style;
    bool visible;
    double x, y, w, h;
    PosRC caretPos, realCaretPos, renderPos, selectLeftPos, selectRightPos;
    UndoRedo *urStack;
    Passage *passage;
} EditorForm;

/*
    Struct: Editor
    Overall struct of an editor, containing several(probably) forms.
*/
typedef struct {
    int fileCount, curSelect;
    double menuHeight, barHeight;
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
static void drawCodeLine(EditorForm* form, Line* line, double x, double y, double w, double h);
static void drawToken(Token* token, double x, double y, double w, double h);
static void drawTokenBox(Token* token, double x, double y, double w, double h);

void addCodeToEditor(Editor* editor, FILE* fp, char* filePath);
static void moveCaret(EditorForm *form, CaretAction action, char* curLine, char* preLine);
void moveCaretByMouse(Editor* editor, int x, int y, int button, int event);
void moveCaretByKey(Editor* editor, int key, int event);
void addChar(char ch);
#endif
