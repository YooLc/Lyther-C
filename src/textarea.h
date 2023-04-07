#ifndef _TEXTAREA_H_
#define _TEXTAREA_H_

#include "doublylinkedlist.h"
#include "linkedlist.h"
#include "codeparser.h"

typedef struct {
    int r, c;
} PosRC;

/*
    Struct: Palette
    Storage colors used for each page.
    Colors and styles are defined in style.h.
*/
typedef struct {
    // Colors for background and text(forground).
    char background[32]; 
    char foreground[32];

    // Colors for line index part on the left.
    char lineIndexBackground[32];
    char lineIndexForeground[32];
    char lineIndexHighlight[32];

    // Colors for different types of keywords.
    // To be implemented...
} Palette;

/*
    Struct: Page
    Single page in the text area, to display single .c file.
*/
//typedef struct {
//    Palette palette;
//
//    double width, height; // Border of the page
//    LinkedList codeStack, undoStack;
//    linkedlistADT codeList;
//} Page;

void drawCodeForm(Passage *passage);
void drawCodeLine(Line* line, double x, double y, double w, double h);
void drawTokenBox(Token* token, double x, double y, double w, double h);
void moveCursor(int key, int event);
void addChar(char ch);
#endif
