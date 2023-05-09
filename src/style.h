#ifndef _STYLE_H_
#define _STYLE_H_

#include "genlib.h"
#include "graphics.h"
#include "codeparser.h"

#define MAX_NAME_LEN 20

/*
    Struct: Palette
    Storage colors used for each page.
    Colors and styles are defined in style.h.
*/
typedef struct {
    char name[MAX_NAME_LEN];
    
    // Colors for background and text(forground).
    char background[MAX_NAME_LEN]; 
    char foreground[MAX_NAME_LEN];

    // Colors for line index part on the left.
    char lineIndexBackground[MAX_NAME_LEN];
    char lineIndexForeground[MAX_NAME_LEN];

    // Colors for different types of keywords.
    char preprocess[MAX_NAME_LEN];
    char bracket[3][MAX_NAME_LEN];
    char quote[MAX_NAME_LEN];
    char comment[MAX_NAME_LEN];
    char keyword[MAX_NAME_LEN];

    // Colors for selection and line highlight
    char selection[MAX_NAME_LEN];
    char highlight[MAX_NAME_LEN];
    
    // Colors for UI
    char caret[MAX_NAME_LEN];
    char frame[MAX_NAME_LEN];
    char label[MAX_NAME_LEN];
    char hotFrame[MAX_NAME_LEN];
    char hotLabel[MAX_NAME_LEN];
} Palette;

void InitStyle();
string getColorByTokenType(CodeTokenType type, int degree);
int getStyleByTokenType(CodeTokenType type);

/*
    Function: defineColorRGB
    Usage: defineColorRGB("Tianyi Blue", "66CCFF");
    Note: It's not case-sensitive.
*/
void defineColorRGB(string name, string color);

#endif
