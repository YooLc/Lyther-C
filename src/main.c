#include <stdio.h>
#include <stdlib.h>

#include "graphics.h"
#include "imgui.h"
#include "textarea.h"
#include "codeparser.h"

void Display(void);

void KeyboardEventProcess(int key, int event)
{
    uiGetKeyboard(key, event);
    Display();
}

void Main() 
{
    SetWindowTitle("Light C code editor");
	InitGraphics();
	InitConsole(); // For debug use. 
	Passage p;
	initPassage(&p);

	addString(&p, "1234", 1, 1);//add "1234" at line 1 column 1
	addString(&p, "abcd", 2, 1);//add "abcd" at line 2 column 1
	addString(&p, "####", 1, 2);//insert "####" at line 1 column 2
    printPassage(&p);
	InitGUI();
	registerKeyboardEvent(KeyboardEventProcess);
}

void Display(void)
{
    DisplayClear();
    //DrawTextArea();
}
