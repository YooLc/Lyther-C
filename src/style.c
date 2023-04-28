#include <string.h>

#include "style.h"
#include "extgraph.h"
#include "codeparser.h"

Palette g_palette[] = {
    {"Classic", 
     "White", "Black",
     "Gray", "Black",
     "Green", {"Red", "Red", "Red"}, "Blue", "Blue",
     "SelectedColor", "Light Blue"},
    {"Classic Plus", 
     "Dark Black", "Light Gray",
     "Dark Black", "Light Gray",
     "Dark Green", {"Very Blue", "Light Yellow", "Light Purple"}, "Brown", "Brown",
     "SelectedColor", "Light Blue"}
};

static int g_selection = 1;

void InitStyle()
{
    defineColorRGB("Dark Black", "1e1e1e");
    defineColorRGB("Light Gray", "d4d4d4");
    defineColorRGB("Light Yellow", "fcd912");
    defineColorRGB("Light Purple", "dd6ed6");
    defineColorRGB("Very Blue", "359cfe");
    defineColorRGB("Dark Green", "679a55") ;
    defineColorRGB("Brown", "cd9279"); 
    defineColorRGB("Light Blue", "ccffff");
    defineColorRGB("SelectedColor", "0984e3");
}

string getColorByTokenType(CodeTokenType type, int degree)
{
    switch(type) {
        case PREPROCESS:
            return g_palette[g_selection].preprocess;
        case LEFT_PARENTHESES: case RIGHT_PARENTHESES:
        case LEFT_BRACKETS: case RIGHT_BRACKETS: 
        case LEFT_BRACE: case RIGHT_BRACE:
            return g_palette[g_selection].bracket[degree % 3];
        case SINGLE_QUOTE: case DOUBLE_QUOTE:
            return g_palette[g_selection].quote;
        default:
            return g_palette[g_selection].foreground;
    }
}

string getBackgroundColor()
{
    return g_palette[g_selection].background;
}

int getStyleByTokenType(CodeTokenType type)
{
    switch(type) {
        case KEYWORD: case DOUBLE_QUOTE: case SINGLE_QUOTE:
            return Bold;
        case COMMENT:
            return Italic;
        default:
            return Normal;
    }
}

void defineColorRGB(string name, string color) {
    int i, value, result[3];
    memset(result, 0, sizeof(result));
    for (i = 0; i < 6; i++) {
        if (isdigit(color[i])) value = color[i] - '0';
        else value = tolower(color[i]) - 'a' + 10;
        result[i / 2] = result[i / 2] * 16 + value;
    }
    DefineColor(name, result[0] / 256.0, result[1] / 256.0, result[2] / 256.0);
}
