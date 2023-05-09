#include "imgui.h"
#include "extgui.h"
#include "graphics.h"
#include "extgraph.h"

#include <stdbool.h>

extern UIState gs_UIState;
extern MenuPalette gs_menu_color;

/* 测试：坐标点(x,y)是否位于包围和 [x1,x2] X [y1,y2] 内部 */
static bool inBox(double x, double y, double x1, double x2, double y1, double y2)
{
	return (x >= x1 && x <= x2 && y >= y1 && y <= y2);
}

static int barItem(int id, double x, double y, double w, double h, char *label, int selected)
{
	char *frameColor = gs_menu_color.frame;
	char *labelColor = gs_menu_color.label;
	if (selected) {
	    frameColor = gs_menu_color.hotFrame;
		labelColor = gs_menu_color.hotLabel;
    }
	if (inBox(gs_UIState.mousex, gs_UIState.mousey, x, x + w, y, y + h)) {
		frameColor = gs_menu_color.hotFrame;
		labelColor = gs_menu_color.hotLabel;
		
		if ( (gs_UIState.clickedItem == id || gs_UIState.clickedItem == 0) && gs_UIState.mousedown) {
			gs_UIState.clickedItem = id;
		}
	}
	else {
		if ( gs_UIState.clickedItem==id )
			gs_UIState.clickedItem = 0;
	}

	mySetPenColor(frameColor);
	drawBox(x, y, w, h, gs_menu_color.fillflag, label, 'L', labelColor);

//    printf("gs_UIState.clickedItem %d, id %d\n", gs_UIState.clickedItem, id);
//    printf("gs_UIState.mousedown: %d\n", gs_UIState.mousedown);
	if( gs_UIState.clickedItem==id && // must be clicked before
		! gs_UIState.mousedown )     // but now mouse button is up
	{
		gs_UIState.clickedItem = 0;
		return 1; 
	}

	return 0;
}

int selectBar(int id, double x, double y, double w, double h, char *labels[], int count, int curSelect)
{
    SetPenColor(gs_menu_color.frame);
    drawRectangle(x, y, w, h, 1);
    // Because there's no layer
//    if( inBox(gs_UIState.mousex, gs_UIState.mousey, x, x + w, y, y + h) )
//		gs_UIState.actingMenu = id;
	
	int i, result = curSelect;
	double labelWidth, curPosX = x;
	for (i = 1; i <= count; i++) {
	    labelWidth = TextStringWidth(labels[i]) + GetFontAscent();
	    if( barItem(GenUIID(i), curPosX, y, labelWidth, h, labels[i], (i == curSelect)) ) {
	        result = i;
        }
	    curPosX += labelWidth;
    }
	return result;
}
