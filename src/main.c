#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "trie.h"
#include "menu.h"
#include "imgui.h"
#include "ioutils.h"
#include "extgraph.h"
#include "graphics.h"
#include "textarea.h"
#include "undoredo.h"
#include "clipboard.h"
#include "codeparser.h"

#define REFRESH_TIMER 1

Editor editor;
UndoRedo undoRedo;

void Display(void)
{
    DisplayClear();
    drawEditor(&editor);
    displayMenu();
}

// To reduce lag, Display() are called only if it's nesessary except timer event.
void KeyboardEventProcess(int key, int event)
{
    uiGetKeyboard(key, event);
    if (!isHelperActivated())
        handleKeyboardEvent(&editor, key, event);
}

void CharEventProcess(char ch)
{
    uiGetChar(ch);
    if (!isHelperActivated())
        handleInputEvent(&editor, ch);
    Display();
}

void MouseEventProcess(int x, int y, int button, int event)
{
    uiGetMouse(x, y, button, event);
    if (!isHelperActivated())
        handleMouseEvent(&editor, x, y, button, event);
    if (event != MOUSEMOVE)
        Display();
}

void TimerEventProcess(int timerID)
{
    if (timerID == REFRESH_TIMER)
        Display();
}

void Main() 
{
    // Initialize libgraphics and imgui
    SetWindowTitle("Light C code editor");
    InitGraphics();
    InitGUI();
    // For debug use.
    InitConsole();

    // The special point size is tofix werid offset when drawing text caused by libgraphics.
    // Note that this value varies to different fonts
    SetFont("Consolas");
    SetPointSize(TEXT_POINT_SIZE);

    // Initialize of data structures used in the editor
    initStyle();
    initEditor(&editor);
    initMenu();
    addCodeToEditor(&editor, NULL, "Untitled.c");

    // Setup callback functions to handle various events
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

