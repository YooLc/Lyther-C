#include "menu.h"
#include "extgraph.h"

void initMenu(){
	menu.activate = 0;
	menu.menuHeight = MENUHEIGHT;
	menu.menuWidth = MENUWIDTH;
	menu.posx = 100;
	menu.posy = 100;
}

void menuGetMouse(int x, int y, int button, int event){
	switch(event){
		case BUTTON_DOWN:
			if(button == RIGHT_BUTTON){
				menu.activate = 1;
				menu.posx = x;
				menu.posy = y;
			}else{
				menu.activate = 0;
			}
	}
}

void displayMenu(){
	if(!menu.activate) return;
	SetPenColor("Gray");
	SetPenSize(1);
	MovePen(ScaleXInches(menu.posx), ScaleYInches(menu.posy));
	StartFilledRegion(1);
	DrawLine(ScaleXInches(menu.menuHeight), 0);
	DrawLine(0, -(double)menu.menuWidth/GetYResolution());
	DrawLine(-ScaleXInches(menu.menuHeight), 0);
	DrawLine(0, (double)menu.menuWidth/GetYResolution());
	EndFilledRegion();
}
