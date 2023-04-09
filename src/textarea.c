#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "textarea.h"
#include "codeparser.h"
#include "style.h"
#include "extgraph.h"
#include "graphics.h"
#include "imgui.h"
#include "undoredo.h"

PosRC g_cursorPos = {0, 0}, g_renderPos;

void drawEditor(Passage *passage, UndoRedo *ur) {
    double winWidth = GetWindowWidth();
    double winHeight = GetWindowHeight();
    drawEditorMenu(ur, winWidth, winHeight);
    drawCodeForm(passage, winWidth, winHeight - GetFontHeight() * 1.5);
}

void drawEditorMenu(UndoRedo *ur, double width, double height) {
    static char* menuListEdit[] = {"Edit",
        "Undo | Ctrl-Z",
        "Redo | Ctrl-Y"};
    int selection;
    double x = 0;
    double y = height;
    double h = GetFontHeight() * 1.5;
    double w = TextStringWidth(menuListEdit[0]) * 2;
    double wlist = TextStringWidth(menuListEdit[1]) * 2;
    double xindent = height / 20;
    drawMenuBar(0, y - h, width, h);
    selection = menuList(GenUIID(0), 0, y - h, w, wlist, h, menuListEdit, sizeof(menuListEdit) / sizeof(menuListEdit[0]));
    switch(selection) {
        case 1: Undo(ur); g_cursorPos.c--; break;
        case 2: Redo(ur); g_cursorPos.c++; break;
    }
} 

void drawCodeForm(Passage *passage, double width, double height) {
    double fontHeight = GetFontHeight();
    double curLinePosY = height - fontHeight;
    g_renderPos.r = g_renderPos.c = 0;
    // printf("Font Height: %.2f  Width: %.15f\n", fontHeight, fontWidth);

    // Traverse passage (list of lines)
 	Listptr curLine = kthNode(&(passage->passList), 1);
 	while(curLine != NULL) {
        // Traverse line (list of tokens)
 		drawCodeLine(curLine->datptr, 0, curLinePosY, width, fontHeight);
 		curLine = curLine->next;
        curLinePosY -= fontHeight;
        g_renderPos.r++;
        g_renderPos.c = 0;
 	}
 	// printf("%d %d", g_cursorPos.r, g_cursorPos.c);
    return;
}

void drawCodeLine(Line* line, double x, double y, double w, double h) {
    Listptr curToken = kthNode(&(line->lineList), 1);
    
    double tokenWidth;
 	double curTokenPosX = 0;
 	if (g_renderPos.r == g_cursorPos.r) {
 	    SetPenColor("Light Blue");
 		drawRectangle(x, y, w, h, 1);
 		if ((clock() >> 8) & 1) {
 		    char tmpstr[MAX_LINE_SIZE] = "\0"; 
 		    int i;
 		    // Pure Brute Force, wait for better implementation.
            for (i = 0; i < g_cursorPos.c; i++) strcat(tmpstr, " ");
            strcat(tmpstr, "|");
            SetPenColor("Black");
            MovePen(x - GetFontAscent() / 4, y + GetFontDescent()); // This is relatively correct, not exact.
            DrawTextString(tmpstr);
        }
    }
    
 	while (curToken != NULL) {
 		Token* token = curToken->datptr;
 		tokenWidth = TextStringWidth(token->content);
 		//printf("Drawing Token: \'%s\' curTokenPosX: %.4f TokenWidth: %.4f Color: %s\n", token->content, curTokenPosX, tokenWidth, getColorByTokenType(token->type));  
 		// Easiest way to implement textbox() with no extra functions
        drawTokenBox(token, curTokenPosX, y, tokenWidth, h);
        g_renderPos.c += token->length;
        curToken = curToken->next;	
        curTokenPosX += tokenWidth;
    }
}

void drawTokenBox(Token* token, double x, double y, double w, double h) {
//    if (token->selected) {
//        SetPenColor("Blue");
//        drawRectangle(x, y, w, h, 1);
//        SetPenColor("White");
//    }
//    else SetPenColor(getColorByTokenType(token->type));
    SetStyle(getStyleByTokenType(token->type));
    SetPenColor(getColorByTokenType(token->type));
    // SetPenColor("Blue");
    MovePen(x, y + GetFontDescent());
    DrawTextString(token->content);
}

void moveCursor(Passage* passage, int key, int event) {
    if (event != KEY_DOWN) return; 
    switch(key) {
        case VK_LEFT:
            if (g_cursorPos.c > 0) g_cursorPos.c--;
            else if (g_cursorPos.r > 0) g_cursorPos.r--;
            break;
        case VK_RIGHT:
            g_cursorPos.c++;
            break;
        case VK_UP:
            if (g_cursorPos.r > 0) g_cursorPos.r--;
            break;
        case VK_DOWN:
            g_cursorPos.r++;
            break;
        case VK_BACK:
            deleteString(passage, g_cursorPos.r + 1, g_cursorPos.c, g_cursorPos.r + 1, g_cursorPos.c);
            g_cursorPos.c--;
            break;
        case VK_DELETE:
            deleteString(passage, g_cursorPos.r + 1, g_cursorPos.c + 1, g_cursorPos.r + 1, g_cursorPos.c + 1);
            break;
        case VK_RETURN:
            addString(passage, "\n", g_cursorPos.r + 1, g_cursorPos.c + 1);
            g_cursorPos.r++;
            break;
    }
}

