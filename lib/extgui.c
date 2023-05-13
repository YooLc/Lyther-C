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

int vertivalScrollBar(int id, double x, double y, double w, double h, int s, int t, int is, int it) 
{
    // If there's no need to show a scroll bar
    if (s == is && t == it) return s;
    
    // Draw gutter
    double gutterX, gutterW;
    gutterW = GUTTER_WIDTH;
    gutterX = x + w - gutterW;
    SetPenColor(gs_menu_color.frame);
    drawRectangle(gutterX, y, gutterW, h, 1);
    
    // Draw bar
    double totInterval =  (double)(t - s + 1), inInterval = (double)(it - is + 1);
    double innerLength = inInterval / totInterval * h;
    double innerHeight = (totInterval - (double)(is - 1)) * h / totInterval - innerLength;
    char* barColor = gs_menu_color.hotFrame;
    
    int scrollDist = 0;
    
    // Handle drag event
    if (inBox(gs_UIState.mousex, gs_UIState.mousey, gutterX + gutterW / 10, gutterX + gutterW * 9 / 10, y + innerHeight, y + innerHeight + innerLength)) {
		barColor = "White";
		
		if (gs_UIState.mousedown) {
		    printf("Mouse is down\n");
		    if (gs_UIState.clickedItem == id) {
		        if (gs_UIState.mousedy < 0) scrollDist = 1;
		        else if (gs_UIState.mousedy > 0) scrollDist = -1;
            }
            else if (gs_UIState.clickedItem == 0) {
                gs_UIState.clickedItem = id;
                printf("Clicked is yes\n");
            }
            gs_UIState.mousedy = 0;
			gs_UIState.mousedx = 0;
        }
	}
	else {
		if (gs_UIState.clickedItem == id)
			gs_UIState.clickedItem = 0;
	}
	
	// Handle roll up/down event
    if (gs_UIState.mouserolldown == 1) {
        scrollDist = SCROLL_DIST;
        gs_UIState.mouserolldown = 0;
    }
    else if (gs_UIState.mouserollup == 1) {
        scrollDist = -SCROLL_DIST;
        gs_UIState.mouserollup = 0;
    }
    
    is += scrollDist;
    is = is > t ? t : is;
    is = is < 1 ? 1 : is;
    
	SetPenColor(barColor);
	drawRectangle(gutterX + gutterW / 10, y + innerHeight, gutterW * 4 / 5, innerLength, 1);
    return is;
}
