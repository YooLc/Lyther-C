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
    form->caretPos.c = form->realCaretPos.c = form->renderPos.c = 0;
    form->x = form->y = 0;
    form->w = GetWindowWidth();
    form->h = GetWindowHeight() - editor->menuHeight - editor->barHeight;
    
    if (fp != NULL) { /* Do sth. here */ }
    else {
        form->passage = NEW(Passage);
        initPassage(form->passage);
        addString(form->passage, "\n", 1, 1);
    }
    form->urStack = NEW(UndoRedo);
    initUndoRedoList(form->urStack, form->passage);
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

static void moveCaret(EditorForm *form, CaretAction action) {
    PosRC curPos = form->realCaretPos;
    
    // Some essential variables to count new position
    int row = (form->passage->passList).listLen;
    Line *curLine, *preLine;
    curLine = kthNode(&(form->passage->passList), curPos.r)->datptr;
    if (form->realCaretPos.r > 1)
        preLine = kthNode(&(form->passage->passList), curPos.r - 1)->datptr;
    
    switch(action) {
        case LEFT:
            if (curPos.c >= 1) curPos.c--;
            else if (curPos.r > 1) {
                curPos.r--; curPos.c = preLine->length;
            }
            break;
        case RIGHT:
            if (curPos.c < curLine->length) curPos.c++;
            else if (curPos.r < row) {
                curPos.r++; curPos.c = 0;
            }
            break;
        case UP:
            if (curPos.r > 1) curPos.r--;
            break;
        case DOWN:
            if (curPos.r < row) curPos.r++;
            break;
    }
    
    // Force update caret position
    form->caretPos = form->realCaretPos = curPos;
    return;
}

static deleteLastChar(EditorForm* form) {
    PosRC curPos = form->realCaretPos;
    if (curPos.c > 0) {
        moveCaret(form, LEFT);
        deleteString(form->passage, curPos.r, curPos.c, curPos.r, curPos.c);
    }
    else if (curPos.r > 1) {
        Line *preLine = kthNode(&(form->passage->passList), curPos.r - 1)->datptr;
        moveCaret(form, LEFT);
        deleteString(form->passage, curPos.r - 1, preLine->length + 1, curPos.r - 1, preLine->length + 1);
    } 
}

void handleMouseEvent(Editor* editor, int x, int y, int button, int event) {
//    if (event != BUTTON_DOWN) return;
//    PosRC newPos = pixelToPosRC(passage, x, y, 6.90);
//    g_cursorPos = g_realPos = newPos;
//    printf("Called move mouse, newPos (%d, %d)\n", newPos.r, newPos.c);
}

void handleInputEvent(Editor* editor, char ch) {
    // The top bit of Chinese characters in GB2312 is 1, so ch is negative
    EditorForm *form = editor->forms[editor->curSelect];
    PosRC curPos = form->realCaretPos;
    if ((ch >= 32 && ch < 127) || ch < 0) {
        char tmpstr[MAX_LINE_SIZE] = "";
        sprintf(tmpstr, "%c", ch);
        addTrace(form->urStack, ADD, curPos.r, curPos.c + 1, curPos.r, curPos.c + 1, tmpstr);
        printf(LOG "Attempting to add %s\n", tmpstr);
        form->caretPos = form->realCaretPos = addString(form->passage, tmpstr, curPos.r, curPos.c + 1);
        printPassage(form->passage);
    }
    printPassage(form->passage);
}
void handleKeyboardEvent(Editor* editor, int key, int event) {
    if (event != KEY_DOWN) return; 
    EditorForm *curForm = editor->forms[editor->curSelect];
    PosRC curPos = curForm->realCaretPos;
    switch(key) {
        case VK_LEFT:  moveCaret(curForm, LEFT); break;
        case VK_RIGHT: moveCaret(curForm, RIGHT); break;
        case VK_UP:    moveCaret(curForm, UP); break;
        case VK_DOWN:  moveCaret(curForm, DOWN); break;
        case VK_BACK:  deleteLastChar(curForm); break;
        case VK_DELETE:
            deleteString(curForm->passage, curPos.r, curPos.c + 1, curPos.r, curPos.c + 1);
            break;
        case VK_RETURN:
            curPos = addString(curForm->passage, "\n", curPos.r, curPos.c + 1);
            break;
    }
    curForm->caretPos = curForm->realCaretPos = curPos;
    // Smart Caret Position
    Line *curLine = kthNode(&(curForm->passage->passList), curPos.r)->datptr;
    curForm->realCaretPos.c = min(curLine->length, curPos.c);
    printf(LOG "Caret at (%d, %d), Real Caret at (%d, %d)\n", 
            curForm->caretPos.r, curForm->caretPos.c, 
            curForm->realCaretPos.r, curForm->realCaretPos.c);
}

