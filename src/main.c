#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "graphics.h"
#include "extgraph.h"
#include "imgui.h"
#include "textarea.h"
#include "codeparser.h"
#include "undoredo.h"
#include "menu.h"
#include "clipboard.h"
#include "trie.h"
#include "ioutils.h"

#define REFRESH_TIMER 1

Editor editor;
UndoRedo undoRedo;

void Display(void);

// To reduce lag, Display() are commented in each event handler
void KeyboardEventProcess(int key, int event)
{
    uiGetKeyboard(key, event);
    if (!isHelperActivated()) handleKeyboardEvent(&editor, key, event);
    //Display();
}

void CharEventProcess(char ch)
{
    uiGetChar(ch);
    if (!isHelperActivated()) handleInputEvent(&editor, ch);
    Display();
}

void MouseEventProcess(int x, int y, int button, int event)
{
    uiGetMouse(x, y, button, event);
    if (!isHelperActivated()) handleMouseEvent(&editor, x, y, button, event);
    if (event != MOUSEMOVE) Display();
}

void TimerEventProcess(int timerID)
{
    if (timerID == REFRESH_TIMER) {
        Display();
    }
}

void Main() 
{
    SetWindowTitle("Light C code editor");
    InitGraphics();
    InitConsole(); // For debug use.
    SetFont("Consolas");
    SetPointSize(22); // This fix werid offset when drawing text. Note that this value varies to different fonts
    InitStyle();
    InitGUI();
    initEditor(&editor);
    addCodeToEditor(&editor, NULL, "Unamed 1.c");
    addCodeToEditor(&editor, NULL, "Unamed 2.c");
//    initUndoRedoList(&undoRedo, &passage);
    //initEditor(&editor);
    
    // A simple test case
//    Passage* passage = editor.forms[1]->passage;
//    addString(passage, "\n", 1, 1);
//    addString(passage, "#include <stdio.h>\n", 1, 1);
//    addString(passage, "void main() { //����ע��\n", 2, 1);
//    addString(passage, "    printf(\"Hello World\"); /*abc*/ \n\n", 3, 1);
//    addString(passage, "}\n", 5, 1);
//    addString(passage, "��� ", 3, 19);
//    addString(passage, " ", 4, 1);
//     addTrace(&undoRedo, ADD, 1, 1, 1, 2, "#i");
//    Undo(&undoRedo);
//    Redo(&undoRedo);
//    printPassage(&passage);
    initMenu();
    startTimer(REFRESH_TIMER, 20);
    registerCharEvent(CharEventProcess);
    registerMouseEvent(MouseEventProcess);
    registerTimerEvent(TimerEventProcess);
    registerKeyboardEvent(KeyboardEventProcess);
    /*
    Tire tire;
    initTire(&tire);
    addStringToTire(tire.root, "abcde");
    addStringToTire(tire.root, "a");
        addStringToTire(tire.root, "abcde");
    deleteStringInTire(tire.root, "abcde");
    TextList *list = matchPrefix(tire.root, "a");
    printf("%s %s %d\n", kthNode(list, 1)->datptr, kthNode(list, 2)->datptr, list->listLen);
	*/
}

void Display(void)
{
    DisplayClear();
    drawEditor(&editor);
    displayMenu();
}
