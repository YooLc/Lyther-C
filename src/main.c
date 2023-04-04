#include <stdio.h>
#include <stdlib.h>

#include "graphics.h"
#include "imgui.h"
#include "textarea.h"

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
	// InitConsole(); // For debug use. 
	InitGUI();
	registerKeyboardEvent(KeyboardEventProcess);

}

void Display(void)
{
    DisplayClear();
    DrawTextArea();
}
