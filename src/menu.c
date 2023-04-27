#include "menu.h"
#include "extgraph.h"
#include "undoredo.h"

void initMenu(){
	menu.activate = 0;
	menu.entryMargin = 0.03;
	menu.entryHeight = GetFontHeight() + 2*menu.entryMargin + GetFontDescent();
	menu.menuHeight = MENUENTRY*menu.entryHeight;
	menu.menuWidth = MENUWIDTH;
	menu.selected = -1;
	menu.posx = 100;
	menu.posy = 100;
	
	DefineColor("MenuBackground", (double)223/256, (double)230/256, (double)233/256);
	DefineColor("MenuSelected", (double)178/256, (double)190/256, (double)195/256);
}

static int inMenu(int x, int y){
	if(!menu.activate) return 0;
	if(x >= menu.posx + menu.menuWidth*GetXResolution() || y >= menu.posy + menu.menuHeight*GetYResolution() || x <= menu.posx || y <= menu.posy){
		menu.selected = -1;
		return 0;
	}
	int entry = (y - menu.posy)/(menu.entryHeight*GetYResolution());
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
				menu.activate = 0;
				switch(menu.selected){
					/*do something here*/
				}
			}else{
				menu.activate = 0;
			}
	}
}

void displayMenu(){
	if(!menu.activate) return;
	int i = 0;
	
	//Draw menu background
	SetPenColor("MenuBackground");
	SetPenSize(1);
	
	MovePen(ScaleXInches(menu.posx), ScaleYInches(menu.posy));
	StartFilledRegion(1);
	DrawLine(menu.menuWidth, 0);
	DrawLine(0, -menu.menuHeight);
	DrawLine(-menu.menuWidth, 0);
	DrawLine(0, menu.menuHeight);
	EndFilledRegion();
	SetPenColor("Black");
	DrawLine(menu.menuWidth, 0);
	DrawLine(0, -menu.menuHeight);
	DrawLine(-menu.menuWidth, 0);
	DrawLine(0, menu.menuHeight);
	SetPenColor("Black");
	for(i=0; i<MENUENTRY; i++){
		MovePen(ScaleXInches(menu.posx), ScaleYInches(menu.posy) - (i+1)*menu.entryHeight + GetFontDescent() + menu.entryMargin);
		DrawTextString(Entrys[i]);
	}

	//Highlight selected entry
	if(menu.selected == -1) return;
	SetPenColor("MenuSelected");
	SetPenSize(1);
	
	MovePen(ScaleXInches(menu.posx) + menu.entryMargin, ScaleYInches(menu.posy) - menu.selected*menu.entryHeight - menu.entryMargin);
	StartFilledRegion(1);
	DrawLine(menu.menuWidth-menu.entryMargin*2, 0);
	DrawLine(0, -(menu.entryHeight-menu.entryMargin*2));
	DrawLine(-menu.menuWidth+menu.entryMargin*2, 0);
	DrawLine(0, menu.entryHeight-menu.entryMargin*2);
	EndFilledRegion();
	
	SetPenColor("White");
	MovePen(ScaleXInches(menu.posx), ScaleYInches(menu.posy) - (menu.selected+1)*menu.entryHeight + GetFontDescent() + menu.entryMargin);
	DrawTextString(Entrys[menu.selected]);
}
