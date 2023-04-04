#include <stdio.h>
#include <stdlib.h>

#include "graphics.h"
#include "imgui.h"

void Main() 
{
    SetWindowTitle("Light C code editor");
	InitGraphics();
	InitGUI();
	return 0;
}
