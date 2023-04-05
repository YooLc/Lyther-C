#include <stdio.h>
#include <string.h>
#include "textarea.h"
#include "codeparser.h"
#include "style.h"
#include "extgraph.h"
#include "graphics.h"

void DrawTextArea(Passage *passage) {
    double winWidth = GetWindowWidth();
    double winHeight = GetWindowHeight();
    
    double fontWidth = GetFontWidth();
    double fontHeight = GetFontHeight();
    double tokenWidth;
    printf("Font Height: %.2f  Width: %.15f\n", fontHeight, fontWidth);
    
    double curLinePosY = winHeight - fontHeight;
    double curTokenPosX = 0;
    
    int idx = 0;
    // Traverse passage (list of lines)
 	Listptr curLine = kthNode(&(passage->passList), 1);
 	while(curLine != NULL) {
        // Traverse line (list of tokens)
 		Line* line = curLine->datptr;
 		Listptr curToken = kthNode(&(line->lineList), 1);
 		curTokenPosX = 0;
 		while (curToken != NULL) {
 		    Token* token = curToken->datptr;
 		    tokenWidth = fontWidth * strlen(curToken->datptr);
 		    printf("Token: \'%s\' curTokenPosX: %.2f TokenWidth: %.2f Color: %s\n", token->content, curTokenPosX, tokenWidth, getColorByTokenType(token->type));
 		    
 		    // Easiest way to implement textbox() with no extra functions
 		    SetPenColor(getColorByTokenType(token->type));
            MovePen(curTokenPosX, curLinePosY);
            DrawTextString(token->content);
            
            curToken = curToken->next;	
            curTokenPosX += tokenWidth;
        }
        putchar('\n');
        curLine = curLine->next;
        curLinePosY -= fontHeight;
 	}
    return;
}
