#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <math.h>
#include <stdbool.h>

#include "textarea.h"
#include "codeparser.h"
#include "style.h"
#include "graphics.h"
#include "extgraph.h"
#include "imgui.h"
#include "undoredo.h"

#define NEW(T) (T*)malloc(sizeof(T))

static double winWidth, winHeight, fontHeight;

void initEditor(Editor* editor) {
    editor->fileCount = 0;
    editor->curSelect = 0;
    editor->filePath[0] = NULL;
    editor->forms[0] = NULL;
    editor->menuHeight = editor->barHeight = GetFontHeight() * 1.5;
    
    winWidth = GetWindowWidth();
    winHeight = GetWindowHeight();
    fontHeight = GetFontHeight();
}

void addCodeToEditor(Editor* editor, FILE* fp, char* filePath) {
    EditorForm *form = NEW(EditorForm);
    editor->fileCount++;
    editor->curSelect = editor->fileCount;
    editor->filePath[editor->fileCount] = filePath;
    editor->forms[editor->fileCount] = form;
    
    form->style = 0;
    form->visible = true;
    form->caretPos.r = form->realCaretPos.r = form->renderPos.r = 1;
    form->caretPos.r = form->realCaretPos.r = form->renderPos.r = 0;
    form->x = form->y = 0;
    form->w = GetWindowWidth();
    form->h = GetWindowHeight() - editor->menuHeight - editor->barHeight;
    form->urStack = NEW(UndoRedo);
    if (fp != NULL) { /* Do sth. here */ }
    else {
        form->passage = NEW(Passage);
        addString(form->passage, "\n", 1, 1);
    }
}

void drawEditor(Editor* editor) {
    drawEditorMenu(editor);
    drawEditorBar(editor);
    int idx;
    for (idx = 1; idx <= editor->fileCount; idx++) {
        if (!editor->forms[idx]->visible) continue;
        drawEditorForm(editor->forms[idx]);
    }
}

static void drawEditorMenu(Editor* editor) {
    static char* menuListFile[] = {"File",
        "Open | Ctrl-O",
        "Save | Ctrl-S",
        "Exit | Ctrl-E"};
    static char* menuListEdit[] = {"Edit",
        "Undo | Ctrl-Z",
        "Redo | Ctrl-Y"};
    int selection;
    double x, y, w, h, wlist, xindent;
    x = 0; y = winHeight - editor->menuHeight;
    w = winWidth; h = editor->menuHeight;
    xindent = h / 20;
    
    drawMenuBar(x, y, w, h);
    // Draw File Menu
    wlist = TextStringWidth(menuListFile[1]) * 2;
    selection = menuList(GenUIID(0), x, y, w, wlist, h, menuListFile, sizeof(menuListFile) / sizeof(menuListFile[0]));
    // Draw Editor Menu
    x += w;
    wlist = TextStringWidth(menuListEdit[1]) * 2;
    selection = menuList(GenUIID(0), x, y, w, wlist, h, menuListEdit, sizeof(menuListEdit) / sizeof(menuListEdit[0]));
    EditorForm* curForm = editor->forms[editor->curSelect];
    switch(selection) {
        case 1: Undo(curForm->urStack); curForm->caretPos.c--; break;
        case 2: Redo(curForm->urStack); curForm->caretPos.c++; break;
    }
} 

static void drawEditorBar(Editor *editor) {
    return;
}

static void drawEditorForm(EditorForm *form) {
    double curLinePosY = form->h - fontHeight;
    
    // Set renderPos at origin, rows are 1-based, and cols are 0-based
    form->renderPos.r = 1;
    form->renderPos.c = 0;
    
    // Traverse passage (list of lines)
 	Listptr curLine = kthNode(&(form->passage->passList), 1);
 	while(curLine != NULL) {
        // Traverse line (list of tokens)
 		drawCodeLine(form, curLine->datptr, 0, curLinePosY, form->w, fontHeight);
 		curLine = curLine->next;
        curLinePosY -= fontHeight;
        form->renderPos.r++;
        form->renderPos.c = 0;
 	}
 	// printf("%d %d", g_cursorPos.r, g_cursorPos.c);
    return;
}


static PosRC pixelToPosRC(Passage *passage, int px, int py, double height) {
    PosRC pos;
    double x = ScaleXInches(px);
    double y = ScaleYInches(py);
    int row = (passage->passList).listLen;
    
    printf("Y: %.2f, height: %.2f", y, height);
    pos.r = max(1, min(row, floor((height - y) / GetFontHeight())));

    char fullLine[MAX_LINE_SIZE];
    getLine(passage, fullLine, pos.r);
    
    pos.c = strlen(fullLine) - 1;
    while (fullLine[pos.c] == '\n') pos.c--;
    while (pos.c >= 0) {
        printf("pos.c : %d, TmpLine is :%s\n", pos.c, fullLine);
        if (TextStringWidth(fullLine) < x) break;
        fullLine[pos.c] = '\0';
        pos.c--;
    }
    pos.c++;
    return pos;
} 

static void drawCodeLine(EditorForm* form, Line* line, double x, double y, double w, double h) {
    double tokenWidth;
 	double curTokenPosX = 0;
 	Listptr curToken = kthNode(&(line->lineList), 1);
 	
 	// If current line is focused...
 	if (form->renderPos.r == form->realCaretPos.r) {
 	    SetPenColor("Light Blue"); // Need to use palette
 		drawRectangle(x, y, w, h, 1);
 		if ((clock() >> 8) & 1) {
 		    char tmpstr[MAX_LINE_SIZE] = "\0"; 
 		    int i;
 		    // Pure Brute Force, wait for better implementation.
            for (i = 0; i < form->realCaretPos.c; i++) strcat(tmpstr, " ");
            strcat(tmpstr, "|");
            SetPenColor("Black");
            MovePen(x - GetFontAscent() / 4, y + GetFontDescent()); // This is relatively correct, not exact.
            DrawTextString(tmpstr);
        }
    }
    
    // Traverse tokens
 	while (curToken != NULL) {
 		Token* token = curToken->datptr;
 		tokenWidth = TextStringWidth(token->content);
 		drawTokenBox(token, curTokenPosX, y, tokenWidth, h);
        form->renderPos.c += token->length;
        curTokenPosX += tokenWidth;
        curToken = curToken->next;
    }
}

static void drawTokenBox(Token* token, double x, double y, double w, double h) {
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

void moveCursorByMouse(Passage* passage, int x, int y, int button, int event) {
    if (event != BUTTON_DOWN) return;
    PosRC newPos = pixelToPosRC(passage, x, y, 6.90);
    g_cursorPos = g_realPos = newPos;
    printf("Called move mouse, newPos (%d, %d)\n", newPos.r, newPos.c);
}

void moveCursor(Passage* passage, int key, int event) {
    if (event != KEY_DOWN) return; 
    int row = (passage->passList).listLen;
    Line *curLine, *preLine;
    curLine = kthNode(&(passage->passList), g_cursorPos.r)->datptr;
    switch(key) {
        case VK_LEFT:
            g_cursorPos = g_realPos;
            if (g_cursorPos.c > 0) g_cursorPos.c--;
            else if (g_cursorPos.r > 1) {
                preLine = kthNode(&(passage->passList), g_cursorPos.r - 1)->datptr;
                g_cursorPos.r--;
                g_cursorPos.c = preLine->length;
            }
            break;
        case VK_RIGHT:
            g_cursorPos = g_realPos;
            if (g_cursorPos.c < curLine->length) g_cursorPos.c++;
            else if (g_cursorPos.r < row) {
                g_cursorPos.r++;
                g_cursorPos.c = 0;
            }
            break;
        case VK_UP:
            if (g_cursorPos.r > 1) g_cursorPos.r--;
            break;
        case VK_DOWN:
            if (g_cursorPos.r < row) g_cursorPos.r++;
            break;
        case VK_BACK:
            g_cursorPos = g_realPos; 
            if (g_cursorPos.c > 0) {
                deleteString(passage, g_realPos.r, g_realPos.c, g_realPos.r, g_realPos.c);
                g_cursorPos.c--;
            }
            else if (g_cursorPos.r > 1) {
                preLine = kthNode(&(passage->passList), g_cursorPos.r - 1)->datptr;
                deleteString(passage, g_cursorPos.r - 1, preLine->length + 1, g_cursorPos.r - 1, preLine->length + 1);
                g_cursorPos.r--;
                g_cursorPos.c = preLine->length;
            } 
            break;
        case VK_DELETE:
            deleteString(passage, g_realPos.r, g_realPos.c + 1, g_realPos.r, g_realPos.c + 1);
            break;
        case VK_RETURN:
            g_cursorPos = g_realPos; 
            addString(passage, "\n", g_realPos.r, g_realPos.c + 1);
            g_cursorPos.r++;
            g_cursorPos.c = 0;
            /* Smart indent
            addString(passage, "\n    ", g_realPos.r, g_realPos.c + 1);
            g_cursorPos.r++;
            g_cursorPos.c = 4; */
            break;
    }
    g_realPos.r = g_cursorPos.r;
    curLine = kthNode(&(passage->passList), g_cursorPos.r)->datptr;
    g_realPos.c = min(curLine->length, g_cursorPos.c);
    printf("cursor at (%d, %d), real pos at (%d, %d)\n", g_cursorPos.r, g_cursorPos.c, g_realPos.r, g_realPos.c);
}

