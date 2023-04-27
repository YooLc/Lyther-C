#include <stdio.h>
#include <stdlib.h>

#include "graphics.h"
#include "extgraph.h"
#include "imgui.h"
#include "textarea.h"
#include "codeparser.h"
#include "undoredo.h"
#include "menu.h"
#include "clipboard.h"

#define REFRESH_TIMER 1

Editor editor;
UndoRedo undoRedo;

void Display(void);

// To reduce lag, Display() are commented in each event handler
void KeyboardEventProcess(int key, int event)
{
    editor.drawLock = true;
    uiGetKeyboard(key, event);
    handleKeyboardEvent(&editor, key, event);
    editor.updated = true;
    editor.drawLock = false;
    // Display();
}

void CharEventProcess(char ch)
{
    editor.drawLock = true;
    uiGetChar(ch);
    handleInputEvent(&editor, ch);
    editor.updated = true;
    editor.drawLock = false;
    // Display();
}

void MouseEventProcess(int x, int y, int button, int event)
{
    editor.drawLock = true;
    uiGetMouse(x, y, button, event);
	menuGetMouse(x, y, button, event);
    handleMouseEvent(&editor, x, y, button, event);
    editor.updated = true;
    editor.drawLock = false;
    // Display();
}

void TimerEventProcess(int timerID)
{
    if (timerID == REFRESH_TIMER)
        Display();
}

void Main() 
{
    SetWindowTitle("Light C code editor");
	InitGraphics();
	InitConsole(); // For debug use.
	SetFont("Consolas");
	SetPointSize(25);
	InitStyle();
	InitGUI();
	initEditor(&editor);
	addCodeToEditor(&editor, NULL, "Unamed 1");
//	initUndoRedoList(&undoRedo, &passage);
	//initEditor(&editor);
    
    // A simple test case
//    Passage* passage = editor.forms[1]->passage;
//    addString(passage, "\n", 1, 1);
//	addString(passage, "#include <stdio.h>\n", 1, 1);
//	addString(passage, "void main() { //中文注释\n", 2, 1);
//	addString(passage, "    printf(\"Hello World\"); /*abc*/ \n\n", 3, 1);
//	addString(passage, "}\n", 5, 1);
//	addString(passage, "这个 ", 3, 19);
//	addString(passage, " ", 4, 1);
//	 addTrace(&undoRedo, ADD, 1, 1, 1, 2, "#i");
//	Undo(&undoRedo);
//	Redo(&undoRedo);
//    printPassage(&passage);
	initMenu();
	startTimer(REFRESH_TIMER, 17); // 60 Hz
	registerCharEvent(CharEventProcess);
	registerMouseEvent(MouseEventProcess);
	registerTimerEvent(TimerEventProcess);
	registerKeyboardEvent(KeyboardEventProcess);
}

void Display(void)
{
    //if (!editor.drawLock && editor.updated) {
        DisplayClear();
        drawEditor(&editor);
        displayMenu();
        editor.updated = false;
    //}
}
