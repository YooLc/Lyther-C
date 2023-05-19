#ifndef _HELPER_H_
#define _HELPER_H_

#include "extgraph.h"

void drawHelper(double x, double y, double w, double h);
void drawAbout(double x, double y, double w, double h);
static void drawPassage(double x, double y, string passage[], int n);

void closeHelper();
void activateHelper();
bool helperActivated();

void closeAbout();
void activateAbout();
bool aboutActivated();


#endif
