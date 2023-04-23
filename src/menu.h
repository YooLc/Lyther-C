#ifndef _MENU_H_
#define _MENU_H_

#define MENUHEIGHT 160
#define MENUWIDTH 100

typedef struct{
	int menuWidth;
	int menuHeight;
	int posx, posy;
	int activate;
	int selected;
}Menu;

Menu menu;

void initMenu();

void menuGetMouse(int x, int y, int button, int event);

void displayMenu();

#endif