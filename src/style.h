#ifndef _STYLE_H_
#define _STYLE_H_

#include "genlib.h"
#include "graphics.h"
#include "codeparser.h"

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
Palette palette[] = {
{"White", "Black", "Gray", "Black", "Light Blue"},
{"Black", "White", "Black", "White", "Gray"}
};

void InitStyle();
string getColorByTokenType(CodeTokenType type);
int getStyleByTokenType(CodeTokenType type);
#endif
