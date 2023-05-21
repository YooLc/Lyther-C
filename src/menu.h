#ifndef _MENU_H_
#define _MENU_H_

#define MENUENTRY 5
#define MENUWIDTH 2.5

#include "textarea.h"

typedef struct {
    double menuWidth;
    double menuHeight;
    double entryHeight, entryMargin;
    int posx, posy;
    int activate;
    int selected;
} Menu;

Menu menu;

static char *menuEntry[] = {" Copy | (C)", " Cut | (X)", " Paste | (V)", " Undo | (Z)", " Redo | (Y)"};

void initMenu();

void menuGetMouse(EditorForm *form, int x, int y, int button, int event);

void displayMenu();

#endif
