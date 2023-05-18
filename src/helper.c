#include <stdio.h>
#include <stdbool.h>

#include "extgraph.h"
#include "imgui.h"
#include "style.h"
#include "helper.h"

static bool isActivated = false;

void drawHelper(double x, double y, double w, double h)
{
    isActivated = true;
    SetPenColor(getBackgroundColor());
    drawRectangle(x, y, w, h, 1);
    MovePen(x, y + GetFontAscent());
    SetPenColor(getForegroundColor());
    SetPointSize(22);
    SetStyle(Bold);
    //DrawTextString("Editing a file");
    //MovePen(w + w / 4, y + h / 6);
    DrawTextString("Editing a file is pretty straightforward. Create (Ctrl+N) or open (Ctrl+O) a file.");
//
//Tips
//Hover over a variable or include line to show information about it.
//Click on a line number to place a breakpoint there.
//Triple click on a line to select it.
//Code completion can be opened using Ctrl+Space anywhere. It will show valid statements found in the current file, its included files and the cache. Preferences can be set at Tools >> Editor Options >> Completion >> Code Completion.
//Symbol completion can be customized at Tools >> Editor Options >> Completion >> Symbol Completion.
//Consider caching frequently included files to speed up file and project opening.");
}

void closeHelper()
{
    isActivated = false;
}

bool isHelperActivated()
{
    return isActivated;
}
