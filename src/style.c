#include "style.h"
#include "extgraph.h"
#include "codeparser.h"

Palette g_palette[] = {
    {"White", "Black", "Gray", "Black", "Light Blue"},
    {"Black", "White", "Black", "White", "Gray"}
};

void InitStyle()
{
    DefineColor("Light Blue", .8, 1, 1);
}

string getColorByTokenType(CodeTokenType type)
{
    switch(type) {
        case PREPROCESS:
            return "Green";
        case LEFT_PARENTHESES: case RIGHT_PARENTHESES: case SEMI_COLON:
            return "Red";
        case LEFT_BRACKETS: case RIGHT_BRACKETS:
            return "Yellow";
        case LEFT_BRACE: case RIGHT_BRACE:
            return "Magenta";
        case SINGLE_QUOTE: case DOUBLE_QUOTE:
            return "Blue";
        default:
            return "Black";
    }
    return "";
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
