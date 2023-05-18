#ifndef _TEXTAREA_H_
#define _TEXTAREA_H_

#include <stdbool.h>
#include "doublylinkedlist.h"
#include "linkedlist.h"
#include "codeparser.h"
#include "undoredo.h"
#include "style.h"

#define MAX_FILE_COUNT 20
#define INDENT_LENGTH 4
#define SCROLL_DIST 3

/*
    Struct: Editor Form
    Single form containing single .c file.
    (x, y) denoted left-bottom point, while (x + w, y + h) is right-top point
    style(int):            the index of style defined in <style.h>
    startLine(int):        the index of first visible line in the screen 
    visible(bool):         is this form visible
    inSelectionMode(bool): is this form in selection mode -> drawEditorSelection()
    selectLeftPos, selectRightPos(PosRC): boundaries for code selection
    caretPos, realCaretPos(PosRC):        caret position in (row, col), real is used for display
    renderPos(PosRC):      position of latest processed and displayed code
    usStack(UndoRedo):     stack used for undo and redo function
    passage(Passage):      basic data structure to storage code
    
*/
typedef struct {
    int style, startLine;
    bool visible, inSelectionMode;
    double x, y, w, h, viewProgress;
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
    char *filePath[MAX_FILE_COUNT], *fileName[MAX_FILE_COUNT];
    EditorForm *forms[MAX_FILE_COUNT];
} Editor;

/*
    Enum: CaretAction
    Wrapped for communicating between functions
*/
typedef enum {
    UP, DOWN, LEFT, RIGHT
} CaretAction;

/*
    Function: initEditor
    Set default values for newly allocated editor
*/
void initEditor(Editor* editor);
/*
    Function: drawEditorMenu
    Display right click menu when it's called
*/
static void drawEditorMenu(Editor* editor);
/*
    Function: drawEditorBar
    Display a file selection bar (like Dev-C++)
*/
static void drawEditorBar(Editor* editor);
/*
    Function: drawEditorForm
    Display a code file with code highlights
*/
static void drawEditorForm(EditorForm* form);
/*
    Function: drawCodeLine
    Display single line of code
*/
static void drawCodeLine(EditorForm* form, Line* line, double x, double y, double w, double h);
/*
    Function: drawToken
    Display single token from parser, with code highlight
*/
static void drawEditorComplete(Editor *editor);
static void drawToken(Token* token, double x, double y, double w, double h);
static void drawEditorSelection(EditorForm* from);
static void drawSymbolMatch(EditorForm *form);
static void drawTokenBox(Token* token, double x, double y, double w, double h);
void drawMessageBar();

void addCodeToEditor(Editor* editor, FILE* fp, char* filePath);
static void drawCaret(EditorForm* form);
static void moveCaret(EditorForm *form, CaretAction action, char* curLine, char* preLine);
void moveCaretByMouse(Editor* editor, int x, int y, int button, int event);
void moveCaretByKey(Editor* editor, int key, int event);
void addChar(char ch);
#endif
