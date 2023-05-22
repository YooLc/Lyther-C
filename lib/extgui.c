#include "imgui.h"
#include "extgui.h"
#include "graphics.h"
#include "extgraph.h"

#include <windows.h> 
#include <stdbool.h>

extern UIState gs_UIState;
extern MenuPalette gs_menu_color;

double barPos[10];

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
		
		printf("acting menu %d\n", gs_UIState.actingMenu ); 
		if (!gs_UIState.actingMenu && (gs_UIState.clickedItem == id || gs_UIState.clickedItem == 0) && gs_UIState.mousedown) {
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
	if( gs_UIState.clickedItem == id && // must be clicked before
		!gs_UIState.mousedown )     // but now mouse button is up
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
    
    //printf("actingMenu %d\n", gs_UIState.actingMenu);
    
    // Because there's no layer
//    if( inBox(gs_UIState.mousex, gs_UIState.mousey, x, x + w, y, y + h) && !gs_UIState.actingMenu)
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
    if (gs_UIState.actingMenu) return curSelect;
	return result;
}

double vertivalScrollBar(int id, double x, double y, double w, double h, double scale, double progress) 
{   
    // Draw gutter
    double gutterX, gutterW;
    gutterW = GUTTER_WIDTH;
    gutterX = x + w - gutterW;
    SetPenColor(gs_menu_color.frame);
    drawRectangle(gutterX, y, gutterW, h, 1);
    
    // Draw bar
    double barLength = scale * h;
    char*  barColor = gs_menu_color.hotFrame;
    double barPos = (1 - progress) * h - barLength;
    
    // Handle drag event
    if (inBox(gs_UIState.mousex, gs_UIState.mousey, gutterX + gutterW / 10, gutterX + gutterW * 9 / 10, y + barPos, y + barPos + barLength)) {
        barColor = "White";
        if (gs_UIState.mousedown && gs_UIState.clickedItem == 0) {
            gs_UIState.clickedItem = id;
            gs_UIState.mousedy = 0;
		    gs_UIState.mousedx = 0;
        }
    }
    
	if (gs_UIState.mousedown && gs_UIState.clickedItem == id) {
	    barColor = "White";
		if (gs_UIState.clickedItem == id)
		    barPos += gs_UIState.mousedy;
        gs_UIState.mousedy = 0;
		gs_UIState.mousedx = 0;
    }
    
	// Handle roll up/down event
    if (gs_UIState.mouserolldown == 1) {
        barPos -= h * scale * 0.15;
        gs_UIState.mouserolldown = 0;
    }
    else if (gs_UIState.mouserollup == 1) {
        barPos += h * scale * 0.15;
        gs_UIState.mouserollup = 0;
    }
    
    // Handle release event
	if (!gs_UIState.mousedown && gs_UIState.clickedItem == id) {
            gs_UIState.clickedItem = 0;
    } 
    
    barPos = max(0, barPos);
	barPos = min(h - barLength, barPos);
    
	SetPenColor(barColor);
	drawRectangle(gutterX + gutterW / 10, y + barPos, gutterW * 4 / 5, barLength, 1);
	progress = (h - barPos - barLength) / h;
    return progress;
}

static int completeItem(int id, double x, double y, double w, double h, char *label)
{
	//puts("ABCD");
	char * frameColor = gs_menu_color.frame;
	char * labelColor = gs_menu_color.label;
	if (inBox(gs_UIState.mousex, gs_UIState.mousey, x, x + w, y, y + h)) {
		frameColor = gs_menu_color.hotFrame;
		labelColor = gs_menu_color.hotLabel;
		if ( (gs_UIState.clickedItem == id ||gs_UIState.clickedItem == 0) && gs_UIState.mousedown) {
			gs_UIState.clickedItem = id;
		}
	}
	else {
		if ( gs_UIState.clickedItem==id )
			gs_UIState.clickedItem = 0;
	}

	mySetPenColor(frameColor);
	drawBox(x, y, w, h, gs_menu_color.fillflag, label, 'L', labelColor);

	if( gs_UIState.clickedItem==id && // must be clicked before
		! gs_UIState.mousedown )     // but now mouse button is up
	{
		gs_UIState.clickedItem = 0;
		return 1; 
	}
	
	return 0;
}

int completeList(int id, double x, double y, double w, double h, char *labels[], int n)
{
	int result = -1;
	// 处理鼠标
	int k;
	
	for( k=0; k<n; k++ ) {
		if ( completeItem(GenUIID(k)+id, x, y-(k+1)*h, w, h, labels[k]) ) {
			result = k;
		}
	}

	return result;
}
