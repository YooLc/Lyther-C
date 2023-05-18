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
#include "extgui.h" 
#include "undoredo.h"
#include "helper.h"

#define NEW(T) (T*)malloc(sizeof(T))

static double winWidth, winHeight;

static int textPointSize, uiPointSize;
static int isControlDown = 0;
static double textFontHeight, uiFontHeight, indexLength;
char g_messageString[250];
static int g_bracketDegree;

extern Palette g_palette[];
extern int g_selection;
extern UIState gs_UIState;

void initEditor(Editor* editor) {
    textPointSize = 22;
    uiPointSize = 15;
    indexLength = 0.5;
    
    winWidth = GetWindowWidth();
    winHeight = GetWindowHeight();
    
    SetPointSize(uiPointSize);
    uiFontHeight = GetFontHeight();
    SetPointSize(textPointSize);
    textFontHeight = GetFontHeight();
    
    editor->fileCount = 0;
    editor->curSelect = 0;
    editor->filePath[0] = NULL;
    editor->forms[0] = NULL;
    editor->menuHeight = editor->barHeight = uiFontHeight * 1.5;
    
    // Test properties
    // editor->drawLock = false;
    // editor->updated = true;
}

static char* getFileName(char* filePath) {
    // To Be Implemented
    int len = strlen(filePath);
    char* fileName = (char *)malloc(sizeof(char) * len);
    
    int idx = len - 1;
    while (idx >= 0 && filePath[idx] != '\\') idx--;
    strcpy(fileName, filePath + 1 + idx);
    return fileName;
}

void addCodeToEditor(Editor* editor, FILE* fp, char* filePath) {
    EditorForm *form = NEW(EditorForm);
    if(editor->fileCount != 0){
        editor->forms[editor->curSelect]->visible = 0;
    }
    editor->fileCount++;
    editor->curSelect = editor->fileCount;
    editor->filePath[editor->fileCount] = filePath;
    editor->fileName[editor->fileCount] = getFileName(filePath);
    editor->forms[editor->fileCount] = form;
    
    form->style = 0;
    form->startLine = 1;
    form->visible = true;
    form->inSelectionMode = false;
    form->caretPos.r = form->realCaretPos.r = form->renderPos.r = 1;
    form->caretPos.c = form->realCaretPos.c = form->renderPos.c = 0;
    form->x = form->y = 0;
    form->w = GetWindowWidth();
    form->h = GetWindowHeight() - editor->menuHeight - editor->barHeight;
    form->viewProgress = 0;
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
    // Setup essential constants
    winWidth = GetWindowWidth();
    winHeight = GetWindowHeight();
    
    SetPointSize(uiPointSize);
    uiFontHeight = GetFontHeight();
    SetPointSize(textPointSize);
    textFontHeight = GetFontHeight();
    indexLength = 0.5 + (textPointSize - 22) / 50.0;
    
    // Reversed drawing to avoid overlaping 
    int idx;
    EditorForm *form;
    for (idx = 1; idx <= editor->fileCount; idx++) {
        if (!editor->forms[idx]->visible) continue;
        form = editor->forms[idx];
        drawEditorForm(form);
        drawEditorSelection(form);
        drawSymbolMatch(form);
        drawCaret(form);
        
        // If it needs a scroll bar
        double scale = form->h / textFontHeight / (form->passage->passList.listLen ? form->passage->passList.listLen : 1);
        if (scale < 1) {
            form->viewProgress = vertivalScrollBar(GenUIID(idx), form->x, form->y, form->w, form->h, scale, form->viewProgress);
            form->startLine = ceil(form->viewProgress * form->passage->passList.listLen) + 1;
            form->startLine = max(1, form->startLine);
        }
    }
    SetPointSize(uiPointSize);
    drawEditorBar(editor);
    drawEditorMenu(editor);
    drawEditorComplete(editor);
    if (isHelperActivated()) {
        drawHelper(0, 0, winWidth, winHeight - editor->menuHeight);
    }
}

static void drawEditorComplete(Editor *editor){
	int offset = -1, i;
	EditorForm *form = editor->forms[editor->curSelect];
	if(form->caretPos.c == 0) return;
	getPos(form->passage, form->caretPos.r, form->caretPos.c+1, &offset);
	if(offset == 0 && form->caretPos.c > 1){
		Token *token = getPos(form->passage, form->caretPos.r, form->caretPos.c, &offset)->datptr;
		if(token->type != STRING) return;
		TextList *list = matchPrefix(form->passage->trie.root, token->content);
		char *labels[MAX_WORD_SIZE];
		if(list == NULL || list->listLen == 0) return;
		for(i=0; i<list->listLen; i++){
			labels[i] = (char *)malloc(sizeof(char)*MAX_WORD_SIZE);
			strcpy(labels[i], token->content);
			strcat(labels[i], kthNode(list, i+1)->datptr);
		}
		completeList(GenUIID(0), (form->caretPos.c+1)*TextStringWidth("a")+indexLength, winHeight-(list->listLen)*editor->menuHeight-(form->caretPos.r)*textFontHeight-editor->barHeight-editor->menuHeight, TextStringWidth("aaaaaaaaaaaaaa"), editor->menuHeight, labels, list->listLen);
		int i;
		for(i=0; i<list->listLen; i++) free(labels[i]);
	}
}

static void drawEditorSelection(EditorForm* form){

    // If selected range is empty or not in selection mode
    if(form->selectLeftPos.r == form->selectRightPos.r && form->selectLeftPos.c == form->selectRightPos.c) return;
    if(form->inSelectionMode == false) return;
    
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
        double x = form->x + indexLength, y = form->h - textFontHeight * (nowRol - form->startLine + 1), w = form->w, h = textFontHeight;
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

static drawCharWithBackground(double x, double y, char* ch){
    SetPenColor("Red");
    drawRectangle(x, y, TextStringWidth("a"), textFontHeight, 1);
    MovePen(x,y + GetFontDescent());
    SetPenColor("White");
    DrawTextString(ch);
}

static int isPosLess(PosRC posx, PosRC posy){
	if(posx.r == posy.r){
		return posx.c < posy.c;
	}
	return posx.r < posy.r;
}

static PosRC matchCharForward(Passage *passage, int rows, int cols, char thisChar, char targetChar){
	PosRC matchTarget = searchForwardByChar(passage, rows, cols, targetChar);
	PosRC matchThis = searchForwardByChar(passage, rows, cols+1, thisChar);
	int degree = 1, i = 0;
	if(matchTarget.r == -1 || matchThis.r == -1) return matchTarget;
	if(isPosLess(matchTarget, matchThis)) return matchTarget;
	matchThis = searchForwardByChar(passage, matchThis.r, matchThis.c+1, thisChar);
	while(isPosLess(matchThis, matchTarget) && matchThis.r != -1){
		degree++;
		matchThis = searchForwardByChar(passage, matchThis.r, matchThis.c+1, thisChar);
		if(matchThis.r == -1) break;
	}
	for(i=0; i<degree; i++){
		matchTarget = searchForwardByChar(passage, matchTarget.r, matchTarget.c+1, targetChar);
		if(matchTarget.r == -1) break;
	}
	return matchTarget;
}

static PosRC matchCharBackward(Passage *passage, int rows, int cols, char thisChar, char targetChar){
	PosRC matchTarget = searchBackwardByChar(passage, rows, cols, targetChar);
	PosRC matchThis = searchBackwardByChar(passage, rows, cols-1, thisChar);
	int degree = 1, i = 0;
	if(matchTarget.r == -1 || matchThis.r == -1) return matchTarget;
	if(!isPosLess(matchTarget, matchThis)) return matchTarget;
	matchThis = searchBackwardByChar(passage, matchThis.r, matchThis.c-1, thisChar);
	while(!isPosLess(matchThis, matchTarget) && matchThis.r != -1){
		degree++;
		matchThis = searchBackwardByChar(passage, matchThis.r, matchThis.c-1, thisChar);
		if(matchThis.r == -1) break;
	}
	for(i=0; i<degree; i++){
		matchTarget = searchBackwardByChar(passage, matchTarget.r, matchTarget.c-1, targetChar);
		if(matchTarget.r == -1) break;
	}
	return matchTarget;
}

static void drawSymbolMatch(EditorForm *form){
    // We just match brackets before the caret
    if (form->realCaretPos.c == 0) return;
    int offset = 0;
    double x1 = form->x + indexLength + (form->realCaretPos.c-1)*TextStringWidth("a"), y1 = form->h - form->realCaretPos.r*textFontHeight;
    double x2 = form->x + indexLength, y2 = form->h;
    PosRC matchPos;
    Token *token = getPos(form->passage, form->realCaretPos.r, form->realCaretPos.c, &offset)->datptr;
    
    if(token->type < LEFT_PARENTHESES || token->type > RIGHT_BRACE) return;
    
    char *typeTable[15] = {"","","","","","","(",")","[","]","{","}"};
    
    if(token->type == LEFT_PARENTHESES || token->type == LEFT_BRACKETS || token->type == LEFT_BRACE){
		matchPos = matchCharForward(form->passage, form->realCaretPos.r, form->realCaretPos.c, *typeTable[token->type], *typeTable[token->type+1]);
		// printf("matchPos F %d %d\n", matchPos.r, matchPos.c);
		x2 += (matchPos.c-1)*TextStringWidth("a");
        y2 -= matchPos.r*textFontHeight;
		drawCharWithBackground(x1, y1, typeTable[token->type]);
        drawCharWithBackground(x2, y2, typeTable[token->type+1]);
	}else{
		matchPos = matchCharBackward(form->passage, form->realCaretPos.r, form->realCaretPos.c, *typeTable[token->type], *typeTable[token->type-1]);
		// printf("matchPos B %d %d\n", matchPos.r, matchPos.c);
		x2 += (matchPos.c-1)*TextStringWidth("a");
        y2 -= matchPos.r*textFontHeight;
        drawCharWithBackground(x1, y1, typeTable[token->type]);
        drawCharWithBackground(x2, y2, typeTable[token->type-1]);
	}

}

static void drawEditorMenu(Editor* editor) {
    Palette* p = g_palette + g_selection;
    setMenuColors(p->frame, p->label, p->hotFrame, p->hotLabel, 1);
    static char* menuListFile[] = {"File",
        "New | Ctrl-N",
        "Open | Ctrl-O",
        "Save | Ctrl-S",
        "Save As...",
        "Switch windows | Ctrl-M",
        "Exit | Ctrl-E"};
    static char* menuListEdit[] = {"Edit",
        "Undo | Ctrl-Z",
        "Redo | Ctrl-Y",
        "Copy | Ctrl-C",
        "Cut  | Ctrl-X",
        "Paste| Ctrl-V"};
    static char* menuListHelp[] = {"Helper",
        "Open Mannual | Ctrl-H"
    }; 
    int selection;
    double x, y, w, h, wlist, xindent;
    x = 0; y = winHeight - editor->menuHeight;
    h = editor->menuHeight;
    xindent = h / 20;
    
    drawMenuBar(x, y, winWidth, h);
    // Draw File Menu
    w = TextStringWidth(menuListFile[0]) * 2;
    wlist = TextStringWidth(menuListFile[5]) * 1.25;
    selection = menuList(GenUIID(0), x, y, w, wlist, h, menuListFile, sizeof(menuListFile) / sizeof(menuListFile[0]));
        switch(selection) {
        case 1: newFile(editor); printf("Clicked\n"); break;
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
    
    PosRC newPos;
    switch(selection) {
        case 1: 
			newPos = Undo(curForm->urStack);
			if(newPos.r != -1) curForm->caretPos = curForm->realCaretPos = newPos;
			break;
        case 2: 
			newPos = Redo(curForm->urStack);
			if(newPos.r != -1) curForm->caretPos = curForm->realCaretPos = newPos;
			break;
        case 3: if(curForm->inSelectionMode) Copy(curForm); break;
        case 4: if(curForm->inSelectionMode) Cut(curForm); break;
        case 5: Paste(curForm); break;
    }
    
    // Draw Helper Menu
    x += w;
    w = TextStringWidth(menuListHelp[0]) * 2;
    wlist = TextStringWidth(menuListHelp[1]) * 1.25;
    selection = menuList(GenUIID(0), x, y, w, wlist, h, menuListHelp, sizeof(menuListHelp) / sizeof(menuListHelp[0]));
    switch(selection) {
        case 1: 
			drawHelper(0, 0, winWidth, winHeight - editor->menuHeight);
			break;
    }
} 

static void drawEditorBar(Editor *editor) {
    double x, y, w, h;
    x = 0; y = winHeight - editor->menuHeight - editor->barHeight;
    w = winWidth; h = editor->barHeight;
    int newSelect = selectBar(GenUIID(0), x, y, w, h, editor->fileName, editor->fileCount, editor->curSelect);
    if (newSelect != editor->curSelect) {
        int idx;
        for (idx = 1; idx <= editor->fileCount; idx++) {
            editor->forms[idx]->visible = (idx == newSelect);
        }
    }
    editor->curSelect = newSelect;
    return;
}

static void drawEditorForm(EditorForm *form) {
    double curLinePosY = form->h - textFontHeight;
    int curLineIndex = 1;
    char lineIndex[10] = "";
    g_bracketDegree = 0;

    SetPenColor(g_palette[g_selection].background);
    drawRectangle(form->x, form->y, form->w, form->h, 1);
    
    // Set renderPos at origin, rows are 1-based, and cols are 0-based
    form->renderPos.r = 1;
    form->renderPos.c = 0;
    
    // Traverse passage (list of lines)
    Listptr curLine = kthNode(&(form->passage->passList), 1);
    while(curLine != NULL) {
        // Draw if and only if it's visible
        if (curLineIndex >= form->startLine && curLinePosY + textFontHeight >= 0) {
            // Draw line index - background 
            SetPenColor(g_palette[g_selection].lineIndexBackground);
            drawRectangle(form->x, curLinePosY, indexLength, textFontHeight, 1);
            sprintf(lineIndex, "%3d", curLineIndex);
            // Draw line index - text
            SetPenColor(g_palette[g_selection].lineIndexForeground);
            if (curLineIndex == form->realCaretPos.r) SetStyle(Bold);
            MovePen(form->x + indexLength - TextStringWidth(lineIndex) - 0.1, curLinePosY + GetFontDescent());
            DrawTextString(lineIndex);
        
            // Traverse line (list of tokens)
            drawCodeLine(form, curLine->datptr, form->x + indexLength, curLinePosY, form->w, textFontHeight);
            curLinePosY -= textFontHeight;
            
        }
        curLine = curLine->next;
        curLineIndex++;
        form->renderPos.r++;
        form->renderPos.c = 0;
    }
    return;
}

static void drawCodeLine(EditorForm* form, Line* line, double x, double y, double w, double h) {
    double tokenWidth;
     double curTokenPosX = x;
     Listptr curToken = kthNode(&(line->lineList), 1);
     // If current line is focused...
     if (form->renderPos.r == form->realCaretPos.r) {
         SetPenColor(g_palette[g_selection].highlight);
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

static bool isCaretVisible(EditorForm *form)
{
    PosRC pos = form->realCaretPos;
    if (pos.r < form->startLine || pos.r > form->startLine + ceil(form->h / textFontHeight))
        return false;
    return true;
} 
 
static void drawCaret(EditorForm *form)
{
    // Only draw Caret when it's visible
    if (!isCaretVisible(form)) return;
    
    int idx;
    double x, y, indent = TextStringWidth("|") / 1.95;
    char fullLine[MAX_LINE_SIZE] = "";
    if ((clock() >> 8) & 1) {
        getLine(form->passage, fullLine, form->realCaretPos.r);
        fullLine[form->realCaretPos.c] = '\0';
        SetPenColor(g_palette[g_selection].caret);
        x = form->x + indexLength + TextStringWidth(fullLine) - indent;
        y = form->h - (form->realCaretPos.r - form->startLine + 1) * textFontHeight;
        MovePen(x, y + GetFontDescent());
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
                if (form->h - textFontHeight * curPos.r >= winHeight)
                    form->h -= textFontHeight;
            }
            break;
        case DOWN:
            if (curPos.r < row) {
                curPos.r++;
                if (form->h - textFontHeight * curPos.r <= 0)
                    form->h += textFontHeight;
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
    char tmpLine[MAX_LINE_SIZE], target[5];
    getLine(form->passage, tmpLine, curPos.r);

    if (curPos.c > 0) {
    	if(tmpLine[curPos.c-1] < 0){
    		strncpy(target, tmpLine+curPos.c-2, 2);
    		target[2] = '\0';
			deleteString(form->passage, curPos.r, curPos.c-1, curPos.r, curPos.c);
			addTrace(form->urStack, DELE, curPos.r, curPos.c-1, curPos.r, curPos.c, target);
		}else{
			target[0] = tmpLine[curPos.c-1];
			target[1] = '\0';
			deleteString(form->passage, curPos.r, curPos.c, curPos.r, curPos.c);
			addTrace(form->urStack, DELE, curPos.r, curPos.c, curPos.r, curPos.c, target);
		}
    }
    else if (curPos.r > 1) {
        Line *preLine = kthNode(&(form->passage->passList), curPos.r - 1)->datptr;
    	addTrace(form->urStack, DELE, curPos.r - 1, preLine->length + 1, curPos.r - 1, preLine->length + 1, "\n");
        deleteString(form->passage, curPos.r - 1, preLine->length + 1, curPos.r - 1, preLine->length + 1);
	} 
}

static PosRC pixelToPosRC(EditorForm *form, int px, int py) {
    double x = ScaleXInches(px);
    double y = ScaleYInches(py);
    Passage *passage = form->passage;
    int row = (passage->passList).listLen;
    
    char fullLine[MAX_LINE_SIZE];
    SetPointSize(textPointSize);	//Restore the font size
    textFontHeight = GetFontHeight();
    PosRC pos;
    pos.r = max(1, form->startLine + (form->h - y) / textFontHeight);
    pos.r = min(pos.r, form->passage->passList.listLen);
    getLine(passage, fullLine, pos.r);
    pos.c = strlen(fullLine) - 1;
    double minDistance = winWidth, dist;
    int col = pos.c;
    
    // Trim CRLF
    while (fullLine[col] == '\r' || fullLine[col] == '\n')
        fullLine[col--] = '\0';
    
    while (col >= -1) {
        dist = fabs(form->x + indexLength + TextStringWidth(fullLine) - x);
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
    if (gs_UIState.clickedItem) return;
    
    static int isLeftButtonDown = 0;
    EditorForm* curForm = editor->forms[editor->curSelect];

    if (ScaleXInches(x) >= curForm->x + curForm->w - GUTTER_WIDTH) return;
    if (ScaleYInches(y) >= winHeight - editor->menuHeight - editor->barHeight) return;

    menuGetMouse(curForm, x, y, button, event);
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
                printf("Selection range: [(%d %d), (%d %d)]\n",curForm->selectLeftPos.r, curForm->selectLeftPos.c,\
                curForm->selectRightPos.r, curForm->selectRightPos.c);
                if( !(
                    curForm->selectRightPos.r == curForm->selectLeftPos.r\
                    &&
                    curForm->selectRightPos.c == curForm->selectLeftPos.c
                )){
                    curForm->inSelectionMode = true;
                }else{
                    curForm->inSelectionMode = false;
                }
                isLeftButtonDown = 0;
            }
            break;
        case MOUSEMOVE:
            if(isLeftButtonDown){
                curForm->selectRightPos = pixelToPosRC(curForm, x, y);
                if( !(
                    curForm->selectRightPos.r == curForm->selectLeftPos.r\
                    &&
                    curForm->selectRightPos.c == curForm->selectLeftPos.c
                )){
                    curForm->inSelectionMode = true;
                }else{
                    curForm->inSelectionMode = false;
                }
                drawEditorSelection(editor->forms[editor->curSelect]);
            }
            break;
        case ROLL_DOWN:
            if (isControlDown) textPointSize -= 2;
            break;
        case ROLL_UP:
            if (isControlDown) textPointSize += 2;
            break;
    }

}

void handleInputEvent(Editor* editor, char ch) {
    // The top bit of Chinese characters in GB2312 is 1, so ch is negative
    EditorForm *form = editor->forms[editor->curSelect];
    PosRC curPos = form->realCaretPos;
    static char lastCn = 0;//track the last Chinese character, if is not, this var is 0
    static bool completed = false;
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
    else if (ch >= 32 && ch < 127) {
        switch(ch) {
            case '}': case ']': case ')':
            case '>': case '\"': case '\'':
                if (completed) {
                    completed = false;
                    return;
                }
        }
        sprintf(tmpstr, "%c", ch);
        addTrace(form->urStack, ADD, curPos.r, curPos.c + 1, curPos.r, curPos.c + 1, tmpstr);
        LOG("Attempting to add %s\n", tmpstr);
        form->caretPos = form->realCaretPos = addString(form->passage, tmpstr, curPos.r, curPos.c + 1);
        curPos = form->realCaretPos;
        
        // Easy version of auto complete
        char ac = 0;
        switch(ch) {
            case '{': ac = '}'; break;
            case '[': ac = ']'; break;
            case '(': ac = ')'; break;
            case '<': ac = '>'; break;
            case '\"': ac = '\"'; break;
            case '\'': ac = '\''; break;
        }
        if (ac) {
            sprintf(tmpstr, "%c", ac);
            addTrace(form->urStack, ADD, curPos.r, curPos.c + 1, curPos.r, curPos.c + 1, tmpstr);
            LOG("Attempting to complete by adding %s\n", tmpstr);
            addString(form->passage, tmpstr, curPos.r, curPos.c + 1);
            completed = true;
        } else completed = false;
    } else if (ch == '\t') {
        // Due to the calculation in addString(), we must use a loop to finish this
        int i;
        for (i = 0; i < INDENT_LENGTH; i++) {
            sprintf(tmpstr, " ");
            addTrace(form->urStack, ADD, curPos.r, curPos.c + 1, curPos.r, curPos.c + 1, tmpstr);
            LOG("Attempting to add [%s]\n", tmpstr);
            form->caretPos = form->realCaretPos = addString(form->passage, tmpstr, curPos.r, curPos.c + 1);
            curPos = form->realCaretPos;
        }
    }
    printPassage(form->passage);
}

void handleKeyboardEvent(Editor* editor, int key, int event) {

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
    if(event == KEY_DOWN){
        switch(key) {
            case VK_LEFT:  moveCaret(curForm, LEFT,  curLine, preLine); break;
            case VK_RIGHT: moveCaret(curForm, RIGHT, curLine, preLine); break;
            case VK_UP:    moveCaret(curForm, UP,    curLine, preLine); break;
            case VK_DOWN:  moveCaret(curForm, DOWN,  curLine, preLine); break;
            case VK_BACK:  
                if(curForm->inSelectionMode == false){
                    deleteLastChar(curForm); 
                    moveCaret(curForm, LEFT,  curLine, preLine); 
                }else{
                    PosRC posl = curForm->selectLeftPos, posr = curForm->selectRightPos;
                    if(posl.r > posr.r || \
                        (posl.r == posr.r && posl.c > posr.c)    
                    ){
                        PosRC tmp = posl;
                        posl = posr;
                        posr = tmp;
                    }
                    char *tmpstr = getString(curForm->passage, posl.r, posl.c+1, posr.r, posr.c);
                    addTrace(curForm->urStack, DELE, posl.r, posl.c+1, posr.r, posr.c, tmpstr);
                    deleteString(curForm->passage,
                                posl.r, posl.c+1,\
                                posr.r, posr.c);
                    free(tmpstr);
                    curForm->inSelectionMode = false;
                }
                break;
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
