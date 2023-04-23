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
    
    //color definition
    DefineColor("SelectedColor", (double)9/256, (double)132/256, (double)227/256);
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
    
    form->passage = NEW(Passage);
    initPassage(form->passage);
    if (fp != NULL) {
		char buf[MAX_LINE_SIZE];
		int idx = 1;
		while(!feof(fp)) {
		    fgets(buf, MAX_LINE_SIZE, fp);
		    addString(form->passage, buf, idx++, 1);
        }	
	}
    else {
        addString(form->passage, "\n", 1, 1);
    }
    form->urStack = NEW(UndoRedo);
    initUndoRedoList(form->urStack, form->passage);
}

void drawEditor(Editor* editor) {
    // Reversed drawing to avoid overlaping 
    int idx;
    for (idx = 1; idx <= editor->fileCount; idx++) {
        if (!editor->forms[idx]->visible) continue;
        drawEditorForm(editor->forms[idx]);
        //printf(LOG "Drawing %d form\n", idx);
    }
    drawEditorBar(editor);
    drawEditorMenu(editor);
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
    h = editor->menuHeight;
    xindent = h / 20;
    
    drawMenuBar(x, y, w, h);
    // Draw File Menu
    w = TextStringWidth(menuListFile[0]) * 2;
    wlist = TextStringWidth(menuListFile[1]) * 1.25;
    selection = menuList(GenUIID(0), x, y, w, wlist, h, menuListFile, sizeof(menuListFile) / sizeof(menuListFile[0]));
        switch(selection) {
        case 1: loadFile(editor); break;
        case 2: saveFile(editor); break;
    }
	// Draw Editor Menu
    x += w;
    w = TextStringWidth(menuListEdit[0]) * 2;
    wlist = TextStringWidth(menuListEdit[1]) * 1.25;
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
    
    //If there is selection in this line
    if(!(form->selectLeftPos.r == form->selectRightPos.r && form->selectLeftPos.c == form->selectRightPos.c)){
    	if(form->renderPos.r >= form->selectLeftPos.r && form->renderPos.r <= form->selectRightPos.r){
			SetPenColor("SelectedColor");
			double yy = y, ww = w;
			if(form->selectLeftPos.r == form->renderPos.r){
				yy = form->selectLeftPos.c*TextStringWidth(" ");
			}else if(form->selectRightPos.r == form->renderPos.r){
				ww = form->selectRightPos.c*TextStringWidth(" ");
			}
 			drawRectangle(x, yy, ww, h, 1);
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

static void moveCaret(EditorForm *form, CaretAction action, char* curLine, char* preLine) {
    PosRC curPos = form->realCaretPos;

    // Some essential variables to count new position
    int row = (form->passage->passList).listLen;
    
    // If it is GB2312 character, we advance by two steps
    switch(action) {
        case LEFT:
            if (curPos.c >= 1) 
                curPos.c -= curLine[curPos.c - 1] < 0 ? 2 : 1;
            else if (curPos.r > 1) {
                curPos.r--; curPos.c = strlen(preLine);
            }
            break;
        case RIGHT:
            if (curPos.c < strlen(curLine)) 
                curPos.c += curLine[curPos.c] < 0 ? 2 : 1;
            else if (curPos.r < row){
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
    printf("New curPos (%d, %d)\n", curPos.r, curPos.c);
    // Force update caret position
    form->caretPos = form->realCaretPos = curPos;
    return;
}

static deleteLastChar(EditorForm* form) {
    PosRC curPos = form->realCaretPos;
    if (curPos.c > 0) {
        deleteString(form->passage, curPos.r, curPos.c, curPos.r, curPos.c);
    }
    else if (curPos.r > 1) {
        Line *preLine = kthNode(&(form->passage->passList), curPos.r - 1)->datptr;
        deleteString(form->passage, curPos.r - 1, preLine->length + 1, curPos.r - 1, preLine->length + 1);
    } 
}

static PosRC pixelToPosRC(EditorForm *form, int px, int py, double height) {
    PosRC pos;
    double x = ScaleXInches(px);
    double y = ScaleYInches(py);
    Passage *passage = form->passage;
    int row = (passage->passList).listLen;
    
    pos.r = max(1, min(row, ceil((height - y) / GetFontHeight())));

    char fullLine[MAX_LINE_SIZE];
    getLine(passage, fullLine, pos.r);
    pos.c = strlen(fullLine) - 1;
    while (fullLine[pos.c] == '\n') pos.c--;
    
    double minDistance = winWidth, dist;
    int col = pos.c;
    while (col >= 0) {
        dist = fabs(TextStringWidth(fullLine) - x);
        if (dist < minDistance) {
            pos.c = col;
            minDistance = dist;
        }
        else break;
        if (fullLine[col] < 0)
            fullLine[col--] = '\0';
        fullLine[col--] = '\0';
    }
    pos.c++;
    return pos;
} 

void handleMouseEvent(Editor* editor, int x, int y, int button, int event) {
	
	static int isLeftButtonDown = 0;
    EditorForm* curForm = editor->forms[editor->curSelect];
    double height = winHeight - (editor->menuHeight + editor->barHeight);

	switch(event){
		case BUTTON_DOWN:
			if(button == LEFT_BUTTON){
				isLeftButtonDown = 1;
				curForm->selectLeftPos = curForm->caretPos = curForm->realCaretPos = pixelToPosRC(curForm, x, y, height);
			}else if(button == RIGHT_BUTTON){
				isLeftButtonDown = 0;
			}
			break;
		case BUTTON_UP:
			if(button == LEFT_BUTTON){
				curForm->selectRightPos = pixelToPosRC(curForm, x, y, height);
				printf("Selection range: [(%d %d), (%d %d)]\n",curForm->selectLeftPos.r, curForm->selectLeftPos.c, \
				curForm->selectRightPos.r, curForm->selectRightPos.c);
				isLeftButtonDown = 0;
			}
			break;
		case MOUSEMOVE:
			if(isLeftButtonDown){
			curForm->selectRightPos = pixelToPosRC(curForm, x, y, height);
				//printf("Selection range: [(%d %d), (%d %d)]\n",curForm->selectLeftPos.r, curForm->selectLeftPos.c, \
				curForm->selectRightPos.r, curForm->selectRightPos.c);
			}
	}
	drawEditor(editor);
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
    // Some essential variables as helper to move caret
    EditorForm *curForm = editor->forms[editor->curSelect];
    PosRC curPos = curForm->realCaretPos;
    char curLine[MAX_LINE_SIZE], preLine[MAX_LINE_SIZE];
    
    // Get current and previous line, then trim all '\n'
    getLine(curForm->passage, curLine, curPos.r);
    int idx = strlen(curLine) - 1;
    while (curLine[idx] == '\n') curLine[idx--] = '\0';
    if (curPos.r > 1) {
        getLine(curForm->passage, preLine, curPos.r - 1);
        idx = strlen(preLine) - 1;
        while (preLine[idx] == '\n') preLine[idx--] = '\0';
    }

    switch(key) {
        case VK_LEFT:  moveCaret(curForm, LEFT,  curLine, preLine); break;
        case VK_RIGHT: moveCaret(curForm, RIGHT, curLine, preLine); break;
        case VK_UP:    moveCaret(curForm, UP,    curLine, preLine); break;
        case VK_DOWN:  moveCaret(curForm, DOWN,  curLine, preLine); break;
        case VK_BACK:  deleteLastChar(curForm); 
                       moveCaret(curForm, LEFT,  curLine, preLine); break;
        case VK_DELETE:
            deleteString(curForm->passage, curPos.r, curPos.c + 1, curPos.r, curPos.c + 1);
            break;
        case VK_RETURN:
            curForm->caretPos = curForm->realCaretPos = addString(curForm->passage, "\n", curPos.r, curPos.c + 1);
            break;
    }
	curPos = curForm->caretPos;	
	/*Function <moveCaret> updates the caretPos and realCaretPos directly, so we need to update <curPos>.
	 The original version is wrong here because VK_RETURN use curPos as the intermediate variable,
	  but LEFT RIGHT change the caretpos directly, does not use curPos as an intermediate variable*/
	  
    // Smart Caret Position, also brute force
    getLine(curForm->passage, curLine, curPos.r);
    int i, col = 0, minDistance = MAX_LINE_SIZE, charWidth;
    for(i = 0; curLine[i]; i += charWidth) {
        charWidth = curLine[i] < 0 ? 2 : 1;
        if (abs(curPos.c - i) < minDistance) {
            col = i;
            minDistance = abs(curPos.c - i);
        }
        else break;
    }
    curForm->realCaretPos.c = col;
    printf("Caret at (%d, %d), Real Caret at (%d, %d)\n", 
            curForm->caretPos.r, curForm->caretPos.c, 
            curForm->realCaretPos.r, curForm->realCaretPos.c);
}
