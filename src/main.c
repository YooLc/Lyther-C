#include <stdio.h>
#include <stdlib.h>

#include "graphics.h"
#include "imgui.h"
#include "textarea.h"
#include "codeparser.h"

#define REFRESH_TIMER 1

Passage passage;
extern PosRC g_cursorPos;
void Display(void);

void KeyboardEventProcess(int key, int event)
{
    uiGetKeyboard(key, event);
    moveCursor(&passage, key, event);
    Display();
}

void CharEventProcess(char ch)
{
    uiGetChar(ch);
    if (ch >= 32 && ch < 127) {
        char tmpstr[MAX_LINE_SIZE] = "";
        sprintf(tmpstr, "%c", ch);
        printf("Attempting to add %s at (%d, %d)\n", tmpstr, g_cursorPos.r + 1, g_cursorPos.c + 1);
        addString(&passage, tmpstr, g_cursorPos.r + 1, g_cursorPos.c + 1);
        g_cursorPos.c++;
        printPassage(&passage);
    }
    Display();
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
	InitStyle();
	
	initPassage(&passage);
	// Same font to the Windows Terminal in Windows 11
	// which is 29 years later than libgraphics
    SetFont("Cascadia Mono");
    // A simple test case
	addString(&passage, "#include <stdio.h>\n", 1, 1);
	addString(&passage, "void main() { //test comment\n", 2, 1);
	addString(&passage, "    printf(\"Hello World\"); /*abc*/ \n\n", 3, 1);
	addString(&passage, "}\n", 5, 1);
	addString(&passage, "this great ", 3, 19);
    printPassage(&passage);
	InitGUI();
	Display();
	startTimer(REFRESH_TIMER, 50);
	registerKeyboardEvent(KeyboardEventProcess);
	registerCharEvent(CharEventProcess);
	registerTimerEvent(TimerEventProcess);
}

void Display(void)
{
    DisplayClear();
    drawCodeForm(&passage);
}
