#ifndef _textarea_h
#define _textarea_h

#include "deque.h"
#include "linkedlist.h"

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
typedef struct {
    Palette palette;

    double width, height; // Border of the page
    Deque codeStack, undoStack;
    linkedlistADT codeList;
} Page;

#endif