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
char g_messageString[250];

static int g_bracketDegree;

void initEditor(Editor* editor) {
    editor->fileCount = 0;
    editor->curSelect = 0;
    editor->filePath[0] = NULL;
    editor->forms[0] = NULL;
    editor->menuHeight = editor->barHeight = GetFontHeight() * 1.5;
    
    // Test properties
    editor->drawLock = false;
    editor->updated = true;
    
    winWidth = GetWindowWidth();
    winHeight = GetWindowHeight();
    fontHeight = GetFontHeight();
}

void addCodeToEditor(Editor* editor, FILE* fp, char* filePath) {
    EditorForm *form = NEW(EditorForm);
    if(editor->fileCount != 0){
		editor->forms[editor->curSelect]->visible = 0;
	}
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
    form->selectLeftPos.r = form->selectLeftPos.c = form->selectRightPos.r = form->selectRightPos.c = 0;
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
    printPassage(form->passage);
}

void drawEditor(Editor* editor) {
    // Reversed drawing to avoid overlaping 
    int idx;
    for (idx = 1; idx <= editor->fileCount; idx++) {
        if (!editor->forms[idx]->visible) continue;
        drawEditorForm(editor->forms[idx]);
        drawEditorSelection(editor->forms[idx]);
        drawCaret(editor->forms[idx]);
        //drawMessageBar();
    }
    drawEditorBar(editor);
    drawEditorMenu(editor);
}

static void drawEditorSelection(EditorForm* form){

	if(form->selectLeftPos.r == form->selectRightPos.r && form->selectLeftPos.c == form->selectRightPos.c) return;
	
	PosRC lRC = form->selectLeftPos, rRC = form->selectRightPos;
	
	if(form->selectLeftPos.r > form->selectRightPos.r ||
				
		(form->selectLeftPos.r == form->selectRightPos.r && 
		form->selectLeftPos.c > form->selectRightPos.c)
					
	){
		PosRC tmpRC = lRC;	
		lRC = rRC;
		rRC = tmpRC;
	}

	if(lRC.r == 0) return;
	
	int nowRol = lRC.r;
	char tmpLine[MAX_LINE_SIZE], targetLine[MAX_LINE_SIZE];

    while(nowRol <= rRC.r){
        double x = form->x + LINE_INDEX_WIDTH, y = form->h - fontHeight * nowRol, w = form->w, h = fontHeight;
    	//Draw background
		SetPenColor("SelectedColor");
		if(lRC.r == nowRol && rRC.r == nowRol){
			x += lRC.c*TextStringWidth("a");
			w = (rRC.c - lRC.c)*TextStringWidth("a");
		}else if(lRC.r == nowRol){
			x += lRC.c*TextStringWidth("a");
		}else if(rRC.r == nowRol){
			w = rRC.c*TextStringWidth("a");
		}
 		drawRectangle(x, y, w, h, 1);
 		//Redraw text
 		SetPenColor("White");
 		getLine(form->passage, tmpLine, nowRol);
 		strcpy(targetLine, tmpLine);
 		if(lRC.r == rRC.r){
			strncpy(targetLine, tmpLine+lRC.c, rRC.c-lRC.c);
			targetLine[rRC.c-lRC.c] = '\0';
		}else if(lRC.r == nowRol){
			strcpy(targetLine, tmpLine + lRC.c);
		}else if(rRC.r == nowRol){
			strncpy(targetLine, tmpLine, rRC.c);
			targetLine[rRC.c] = '\0';
		}
 		MovePen(x, y + GetFontDescent());
 		DrawTextString(targetLine);
 		nowRol++;
	}
	
}

static void drawEditorMenu(Editor* editor) {
    static char* menuListFile[] = {"File",
    	"New | Ctrl-N",
        "Open | Ctrl-O",
        "Save | Ctrl-S",
        "Save As...",
        "Switch windows | Ctrl-M",
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
        case 1: newFile(editor); break;
        case 2: loadFile(editor); break;
        case 3: saveFile(editor); break;
        case 4: saveAs(editor); break;
        case 5: 
        	editor->forms[editor->curSelect]->visible = 0;
			editor->curSelect = (editor->curSelect)%editor->fileCount+1;
			editor->forms[editor->curSelect]->visible = 1;
			drawEditor(editor);
			break;
        case 6: exit(0); break;
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
    int curLineIndex = 1;
    char lineIndex[10] = "";
    g_bracketDegree = 0;

    SetPenColor(getBackgroundColor());
    drawRectangle(form->x, form->y, form->w, form->h, 1);
    
    // Set renderPos at origin, rows are 1-based, and cols are 0-based
    form->renderPos.r = 1;
    form->renderPos.c = 0;
    
    // Traverse passage (list of lines)
 	Listptr curLine = kthNode(&(form->passage->passList), 1);
 	while(curLine != NULL) {
 	    // Draw if and only if it's visible
 	    if (curLinePosY - fontHeight <= winHeight && curLinePosY + fontHeight >= 0) {
 	        // Draw line index
            SetPenColor("Light Gray");
            drawRectangle(form->x, curLinePosY, LINE_INDEX_WIDTH, fontHeight, 1);
            sprintf(lineIndex, "%3d", curLineIndex);
            SetPenColor("Black");
            MovePen(form->x + LINE_INDEX_WIDTH - TextStringWidth(lineIndex), curLinePosY + GetFontDescent());
            DrawTextString(lineIndex);
        
            // Traverse line (list of tokens)
            drawCodeLine(form, curLine->datptr, form->x + LINE_INDEX_WIDTH, curLinePosY, form->w, fontHeight);
        }
        curLine = curLine->next;
        curLinePosY -= fontHeight;
        curLineIndex++;
        form->renderPos.r++;
        form->renderPos.c = 0;
 	}
 	// printf("%d %d", g_cursorPos.r, g_cursorPos.c);
    return;
}

static void drawCodeLine(EditorForm* form, Line* line, double x, double y, double w, double h) {
    double tokenWidth;
 	double curTokenPosX = x;
 	Listptr curToken = kthNode(&(line->lineList), 1);
 	// If current line is focused...
 	if (form->renderPos.r == form->realCaretPos.r) {
 	    SetPenColor("Light Blue"); // Need to use palette
 		drawRectangle(x, y, w, h, 1);
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
    SetStyle(getStyleByTokenType(token->type));
    switch(token->type) {
        case LEFT_BRACKETS: case LEFT_PARENTHESES: case LEFT_BRACE:
            SetPenColor(getColorByTokenType(token->type, ++g_bracketDegree)); break;
        case RIGHT_BRACKETS: case RIGHT_PARENTHESES: case RIGHT_BRACE:
            SetPenColor(getColorByTokenType(token->type, g_bracketDegree--)); break;
        default:
            SetPenColor(getColorByTokenType(token->type, 0)); break;
    }
    // SetPenColor("Blue");
    MovePen(x, y + GetFontDescent());
    DrawTextString(token->content);
}

void drawMessageBar(){
	SetPointSize(15);
	MovePen(winWidth-2, winHeight-GetFontHeight());
	SetPenColor("Black");
    DrawTextString(g_messageString);
	SetPointSize(22);
}

static void drawCaret(EditorForm *form)
{
    int idx;
    double x, y, indent = TextStringWidth("|") / 1.95;
    char fullLine[MAX_LINE_SIZE] = "";
    if ((clock() >> 8) & 1) {
     	getLine(form->passage, fullLine, form->realCaretPos.r);
     	fullLine[form->realCaretPos.c] = '\0';
        SetPenColor("Black");
        x = form->x + LINE_INDEX_WIDTH + TextStringWidth(fullLine) - indent;
        y = form->h - fontHeight * form->realCaretPos.r;
        MovePen(x, y + GetFontDescent()); // This is relatively correct, not exact.
        DrawTextString("|");
    }
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
            if (curPos.r > 1) {
                curPos.r--;
                if (form->h - fontHeight * curPos.r >= winHeight)
                    form->h -= fontHeight;
            }
            break;
        case DOWN:
            if (curPos.r < row) {
                curPos.r++;
                if (form->h - fontHeight * curPos.r <= 0)
                    form->h += fontHeight;
            }
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

static PosRC pixelToPosRC(EditorForm *form, int px, int py) {
    double x = ScaleXInches(px);
    double y = ScaleYInches(py);
    Passage *passage = form->passage;
    int row = (passage->passList).listLen;
    char fullLine[MAX_LINE_SIZE];
    
    
    PosRC pos;
    pos.r = max(1, min(row, ceil((form->h - y) / fontHeight)));
    getLine(passage, fullLine, pos.r);
    pos.c = strlen(fullLine) - 1;

    double minDistance = winWidth, dist;
    int col = pos.c;
    
    // Trim CRLF
    while (fullLine[col] == '\r' || fullLine[col] == '\n')
        fullLine[col--] = '\0';
    
    while (col >= -1) {
        dist = fabs(form->x + LINE_INDEX_WIDTH + TextStringWidth(fullLine) - x);
        if (dist < minDistance) {
            pos.c = col;
            minDistance = dist;
        }
        else break;
        if (col < 0) break;
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

	switch(event){
		case BUTTON_DOWN:
			if(button == LEFT_BUTTON) {
				isLeftButtonDown = 1;
				curForm->selectLeftPos = curForm->selectRightPos = 
                curForm->caretPos = curForm->realCaretPos = pixelToPosRC(curForm, x, y);
			} else if(button == RIGHT_BUTTON) {
				isLeftButtonDown = 0;
			}
			break;
		case BUTTON_UP:
			if(button == LEFT_BUTTON){
				curForm->selectRightPos = pixelToPosRC(curForm, x, y);
				printf("Selection range: [(%d %d), (%d %d)]\n",curForm->selectLeftPos.r, curForm->selectLeftPos.c, 
				curForm->selectRightPos.r, curForm->selectRightPos.c);
				isLeftButtonDown = 0;
			}
			break;
		case MOUSEMOVE:
			if(isLeftButtonDown){
				curForm->selectRightPos = pixelToPosRC(curForm, x, y);
				drawEditorSelection(editor->forms[editor->curSelect]);
			}
			break;
		case ROLL_DOWN:
		    curForm->h += SCROLL_DIST;
		    break;
		case ROLL_UP:
		    curForm->h -= SCROLL_DIST;
		    break;
	}

}

void handleInputEvent(Editor* editor, char ch) {
    // The top bit of Chinese characters in GB2312 is 1, so ch is negative
    EditorForm *form = editor->forms[editor->curSelect];
    PosRC curPos = form->realCaretPos;
    static char lastCn = 0;//track the last Chinese character, if is not, this var is 0
    char tmpstr[MAX_LINE_SIZE] = "";
    
	if(ch < 0){
		if(lastCn == 0){
			lastCn = ch;
		}else{
			sprintf(tmpstr, "%c%c\0", lastCn, ch);
			addTrace(form->urStack, ADD, curPos.r, curPos.c+1, curPos.r, curPos.c+2, tmpstr);
			LOG("Attempting to add %s\n", tmpstr);
        	form->caretPos = form->realCaretPos = addString(form->passage, tmpstr, curPos.r, curPos.c+1);
			lastCn = 0;
		}
	}
    if (ch >= 32 && ch < 127) {
        sprintf(tmpstr, "%c", ch);
        addTrace(form->urStack, ADD, curPos.r, curPos.c + 1, curPos.r, curPos.c + 1, tmpstr);
        LOG("Attempting to add %s\n", tmpstr);
        form->caretPos = form->realCaretPos = addString(form->passage, tmpstr, curPos.r, curPos.c + 1);
        printPassage(form->passage);
    }
    printPassage(form->passage);
}

void handleKeyboardEvent(Editor* editor, int key, int event) {

    // Some essential variables as helper to move caret
    EditorForm *curForm = editor->forms[editor->curSelect];
    PosRC curPos = curForm->realCaretPos;
    char curLine[MAX_LINE_SIZE], preLine[MAX_LINE_SIZE];
    
    //Some status variable
    static isControlDown = 0;
    
    // Get current and previous line, then trim all '\n'
    getLine(curForm->passage, curLine, curPos.r);
    int idx = strlen(curLine) - 1;
    while (curLine[idx] == '\n') curLine[idx--] = '\0';
    if (curPos.r > 1) {
        getLine(curForm->passage, preLine, curPos.r - 1);
        idx = strlen(preLine) - 1;
        while (preLine[idx] == '\n') preLine[idx--] = '\0';
    }
	if(event == KEY_DOWN){
	    switch(key) {
	        case VK_LEFT:  moveCaret(curForm, LEFT,  curLine, preLine); break;
	        case VK_RIGHT: moveCaret(curForm, RIGHT, curLine, preLine); break;
	        case VK_UP:    moveCaret(curForm, UP,    curLine, preLine); break;
	        case VK_DOWN:  moveCaret(curForm, DOWN,  curLine, preLine); break;
	        case VK_BACK:  deleteLastChar(curForm); 
	                       moveCaret(curForm, LEFT,  curLine, preLine); break;
	        case VK_DELETE:
	        	if(curPos.c == strlen(curLine)) curLine[curPos.c] = '\n';
	        	curLine[curPos.c+1] = '\0';
	        	addTrace(curForm->urStack, DELE, curPos.r, curPos.c+1, curPos.r, curPos.c+1, curLine+curPos.c);
	            deleteString(curForm->passage, curPos.r, curPos.c + 1, curPos.r, curPos.c + 1);
	            break;
	        case VK_RETURN:
	        	addTrace(curForm->urStack, ADD, curPos.r, curPos.c+1, curPos.r, curPos.c+1, "\n");
	            curForm->caretPos = curForm->realCaretPos = addString(curForm->passage, "\n", curPos.r, curPos.c + 1);
	            break;
	        case VK_CONTROL:
	        	isControlDown = 1;
	        	break;
		}
    }else if(event == KEY_UP){
		switch(key){
			case VK_CONTROL:
				isControlDown = 0;
				break;
		}
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
