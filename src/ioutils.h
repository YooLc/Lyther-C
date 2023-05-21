#ifndef _IO_UTILS_H_
#define _IO_UTILS_H_

#include "extgraph.h"
#include "textarea.h"

/*
    Part I: File Utilities
*/
/*
    Function: getFileName
    Get file name from its Windows-styled absolute path.
*/
string getFileName(char *filePath);
/*
    Function: NewFile(Editor *editor)
    Creat a new C file, named as <Untitled>
*/
void newFile(Editor *editor);
/*
    Function: loadFile(Editor *editor)
    load <filename> into editor, and rewind the <curselect> to this file
*/
void loadFile(Editor *editor);
/*
    Function: saveAs(Editor *editor)
    Save <curselect> in editor as <fileName>
*/
void saveAs(Editor *editor);
void saveFile(Editor *editor);

/*
    Part II: Keyboard Utilities
*/
/*
    Function: moveCaret
    Handle caret movement (UP, DOWN, LEFT, RIGHT), behaves like common text editor.
    With Chinese(or extended ASCII) characters support.
*/
static void moveCaret(EditorForm *form, CaretAction action, char *curLine, char *preLine);
/*
    Function: deleteLastChar
    Delete the last char before real caret.
*/
static void deleteLastChar(EditorForm *form);
void handleKeyboardEvent(Editor *editor, int key, int event);
void handleInputEvent(Editor *editor, char ch);

/*
    Part III: Mouse Utilities
*/
/*
    Function: pixelToPosRC
    Convert mouse position (pixelX, pixelY) to caret position (row, col).
*/
static PosRC pixelToPosRC(EditorForm *form, int px, int py);
/*
    Function: handleMouseEvent
    Right click - display right click menu
    Left  click - Select a range of texts
    Ctrl + Roll - Alter font size
    This function quits when ui elements in IMGUI is activated to avoid overlapping.
*/
void handleMouseEvent(Editor *editor, int x, int y, int button, int event);

#endif
