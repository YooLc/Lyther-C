#include <stdio.h>
#include <stdlib.h>

#include "graphics.h"
#include "imgui.h"
#include "textarea.h"
#include "codeparser.h"

Passage passage;

void Display(void);

void KeyboardEventProcess(int key, int event)
{
    uiGetKeyboard(key, event);
    Display();
}

void CharEventProcess(char ch)
{
    uiGetChar(ch);
    Display();
}
void Main() 
{
    SetWindowTitle("Light C code editor");
	InitGraphics();
	InitConsole(); // For debug use. 
	
	initPassage(&passage);
	// Same font to the Windows Terminal in Windows 11
	// which is 29 years later than libgraphics
    SetFont("Cascadia Code");
    // A simple test case
	addString(&passage, "#include <stdio.h>\n", 1, 1);
	addString(&passage, "void main() { //test comment\n", 2, 1);
	addString(&passage, "    printf(\"Hello World\"); /*abc*/ \n\n", 3, 1);
	addString(&passage, "}\n", 5, 1);
	addString(&passage, "this great ", 3, 19);
    printPassage(&passage);
	InitGUI();
	Display();
	registerKeyboardEvent(KeyboardEventProcess);
	registerCharEvent(CharEventProcess);
}

void Display(void)
{
    DisplayClear();
    drawCodeForm(&passage);
}
