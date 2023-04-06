#include "style.h"
#include "codeparser.h"

string getColorByTokenType(TokenType type)
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