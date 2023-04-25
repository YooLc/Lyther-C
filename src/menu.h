#ifndef _MENU_H_
#define _MENU_H_

#define MENUENTRY 3
#define MENUWIDTH 2.5

typedef struct{
	double menuWidth;
	double menuHeight;
	double entryHeight, entryMargin;
	int posx, posy;
	int activate;
	int selected;
}Menu;

Menu menu;

static char* Entrys[] = {" Copy | (C)"," Undo | (Z)", " Redo | (Y)"};

void initMenu();

void menuGetMouse(int x, int y, int button, int event);

void displayMenu();

#endif
