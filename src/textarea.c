#include <stdio.h>
#include <string.h>
#include "textarea.h"
#include "codeparser.h"
#include "style.h"
#include "extgraph.h"
#include "graphics.h"

void drawCodeForm(Passage *passage) {
    double winWidth = GetWindowWidth();
    double winHeight = GetWindowHeight();
    double fontHeight = GetFontHeight();
    double curLinePosY = winHeight - fontHeight;
    // printf("Font Height: %.2f  Width: %.15f\n", fontHeight, fontWidth);

    // Traverse passage (list of lines)
 	Listptr curLine = kthNode(&(passage->passList), 1);
 	while(curLine != NULL) {
        // Traverse line (list of tokens)
 		drawCodeLine(curLine->datptr, 0, curLinePosY, winWidth, fontHeight);
 		curLine = curLine->next;
        curLinePosY -= fontHeight;
 	}
    return;
}

void drawCodeLine(Line* line, double x, double y, double w, double h) {
    Listptr curToken = kthNode(&(line->lineList), 1);
    double fontWidth = GetFontWidth();
    double tokenWidth;
 	double curTokenPosX = 0;
 	while (curToken != NULL) {
 		Token* token = curToken->datptr;
 		tokenWidth = fontWidth * strlen(curToken->datptr);
 		// printf("Token: \'%s\' curTokenPosX: %.2f TokenWidth: %.2f Color: %s\n", token->content, curTokenPosX, tokenWidth, getColorByTokenType(token->type));
 		    
 		// Easiest way to implement textbox() with no extra functions
 		drawTokenBox(token, curTokenPosX, y, tokenWidth, h);
        curToken = curToken->next;	
        curTokenPosX += tokenWidth;
    }
    putchar('\n'); 
}

void drawTokenBox(Token* token, double x, double y, double w, double h) {
    SetPenColor(getColorByTokenType(token->type));
    MovePen(x, y);
    DrawTextString(token->content);
}
