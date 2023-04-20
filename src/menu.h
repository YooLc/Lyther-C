#ifndef _MENU_H_
#define _MENU_H_

#define MENUHEIGHT 100
#define MENUWIDTH 50

typedef struct{
	int menuWidth;
	int menuHeight;
	int posx, posy;
	int activate;
}Menu;

Menu menu;

void initMenu();

void menuGetMouse(int x, int y, int button, int event);

void displayMenu();

#endif
