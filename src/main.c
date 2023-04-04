#include <stdio.h>
#include <stdlib.h>

#include "graphics.h"
#include "imgui.h"
#include "textarea.h"

void KeyboardEventProcess(int key, int event)
{
    uiGetKeyborad(key, event);
    Display();
}

void Display(void);

void Main() 
{
    SetWindowTitle("Light C code editor");
	InitGraphics();
	// InitConsole(); // For debug use. 
	InitGUI();
	
	registerKeyboardEvent(KeyboradEventProcess);
	return 0;
}

void Display(void)
{
    DisplayClear();
    DrawTextArea();
}
