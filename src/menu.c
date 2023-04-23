#include "menu.h"
#include "extgraph.h"

void initMenu(){
	menu.activate = 0;
	menu.menuHeight = MENUHEIGHT;
	menu.menuWidth = MENUWIDTH;
	menu.selected = -1;
	menu.posx = 100;
	menu.posy = 100;
	
	DefineColor("MenuBackground", (double)223/256, (double)230/256, (double)233/256);
	DefineColor("MenuSelected", (double)178/256, (double)190/256, (double)195/256);
}

/*
----->x
'
'
'
v
y
*/
static int inMenu(int x, int y){
	if(!menu.activate) return 0;
	if(x >= menu.posx + menu.menuWidth || y >= menu.posy + menu.menuHeight || x <= menu.posx || y <= menu.posy){
		menu.selected = -1;
		return 0;
	}
	int entry = (y - menu.posy)/20;
	menu.selected = entry;
	return 1;
}

void menuGetMouse(int x, int y, int button, int event){
	inMenu(x,y);
	switch(event){
		case BUTTON_DOWN:
			if(button == RIGHT_BUTTON){
				menu.activate = 1;
				menu.posx = x;
				menu.posy = y;
			}else if(button == LEFT_BUTTON && menu.selected != -1){
				/*do something here...*/
			}else{
				menu.activate = 0;
			}
	}
}

void displayMenu(){
	if(!menu.activate) return;
	//Draw menu background
	SetPenColor("MenuBackground");
	SetPenSize(1);
	MovePen(ScaleXInches(menu.posx), ScaleYInches(menu.posy));
	StartFilledRegion(1);
	DrawLine(ScaleXInches(menu.menuWidth), 0);
	DrawLine(0, -(double)menu.menuHeight/GetYResolution());
	DrawLine(-ScaleXInches(menu.menuWidth), 0);
	DrawLine(0, (double)menu.menuHeight/GetYResolution());
	EndFilledRegion();
	//Highlight selected entry
	if(menu.selected == -1) return;
	SetPenColor("MenuSelected");
	SetPenSize(1);
	MovePen(ScaleXInches(menu.posx), ScaleYInches(menu.posy + menu.selected * 20));
	StartFilledRegion(1);
	DrawLine(ScaleXInches(menu.menuWidth), 0);
	DrawLine(0, -(double)20/GetYResolution());
	DrawLine(-ScaleXInches(menu.menuWidth), 0);
	DrawLine(0, (double)20/GetYResolution());
	EndFilledRegion();
}
