#ifndef __extgui_h__
#define __extgui_h__

#include "imgui.h"

#define GUTTER_WIDTH .2
#define SCROLL_DIST  3

static int barItem(int id, double x, double y, double w, double h, char *label, int selected);
int selectBar(int id, double x, double y, double w, double h, char *labels[], int count,
              int curSelect);
double vertivalScrollBar(int id, double x, double y, double w, double h, double scale,
                         double progress);
static int completeItem(int id, double x, double y, double w, double h, char *label);
int completeList(int id, double x, double y, double w, double h, char *labels[], int n);

#endif
