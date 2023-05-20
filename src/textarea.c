#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>

#include "help.h"
#include "imgui.h"
#include "style.h"
#include "extgui.h"
#include "graphics.h"
#include "extgraph.h"
#include "textarea.h"
#include "undoredo.h"
#include "codeparser.h"

#define NEW(T) (T*)malloc(sizeof(T))

double winWidth, winHeight;
int    textPointSize, uiPointSize;
double textFontHeight, uiFontHeight, indexLength;
char   g_messageString[250];

extern Palette g_palette[];
extern int     g_selection;
extern UIState gs_UIState;

void initEditor(Editor* editor) {
    textPointSize = TEXT_POINT_SIZE;
    uiPointSize   = UI_POINT_SIZE;
    indexLength   = LINE_INDEX_LENGTH;
    
    winWidth  = GetWindowWidth();
    winHeight = GetWindowHeight();
    
    SetPointSize(uiPointSize);
    uiFontHeight   = GetFontHeight();
    SetPointSize(textPointSize);
    textFontHeight = GetFontHeight();
    
    editor->fileCount = 0;
    editor->curSelect = 0;
    editor->forms[0]  = NULL;
    editor->filePath[0] = NULL;
    editor->menuHeight  = editor->barHeight = uiFontHeight * 1.5;
}

void addCodeToEditor(Editor* editor, FILE* fp, char* filePath) {
    EditorForm *form = NEW(EditorForm);
    if(editor->fileCount != 0){
        editor->forms[editor->curSelect]->visible = 0;
    }
    editor->fileCount++;
    editor->curSelect = editor->fileCount;
    editor->filePath[editor->fileCount] = (char *)malloc(sizeof(char)*(strlen(filePath)+1));
    strcpy(editor->filePath[editor->fileCount], filePath);
    editor->fileName[editor->fileCount] = getFileName(filePath);
    editor->forms[editor->fileCount] = form;
    
    form->style = 0;
    form->startLine = 1;
    form->completeMode = 0;
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
            if (feof(fp)) break;
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
    
    drawHelper(0, 0, winWidth, winHeight - editor->menuHeight);
    drawAbout(0, 0, winWidth, winHeight - editor->menuHeight);
    
    drawEditorMenu(editor);
    drawEditorComplete(editor);
}

static void drawEditorComplete(Editor *editor){
	int offset = -1, i;
	EditorForm *form = editor->forms[editor->curSelect];
	if(form->realCaretPos.c == 0) return;
	getPos(form->passage, form->realCaretPos.r, form->realCaretPos.c+1, &offset);
	//printf("OFFSET %d\n", offset);
	if(offset == 0 && form->realCaretPos.c >= 1){
		Token *token = getPos(form->passage, form->realCaretPos.r, form->realCaretPos.c, &offset)->datptr;
		if(token->type != STRING){
			form->completeMode = 0;
			return;
		}
		TextList *list = matchPrefix(form->passage->trie.root, token->content);

		char *labels[MAX_WORD_SIZE];
		//if(list == NULL) printf("NULL %s", token->content);
		if(list == NULL || list->listLen == 0){
			form->completeMode = 0;
			return;
		}

		for(i=0; i<list->listLen; i++){
			labels[i] = (char *)malloc(sizeof(char)*MAX_WORD_SIZE);
			strcpy(labels[i], token->content);
			strcat(labels[i], kthNode(list, i+1)->datptr);
			//printf("COMP STR %s\n", labels[i]);
		}
		SetPointSize(textPointSize);
		double listx, listy, listw, listh;
		listx = (form->realCaretPos.c)*TextStringWidth("a")+indexLength;
		listy = winHeight-editor->barHeight-(-form->startLine+form->realCaretPos.r+2)*textFontHeight;
		listw = TextStringWidth("aaaaaaaaaaaaaa");
		listh = editor->menuHeight;

		int selection = completeList(GenUIID(0), listx, listy, listw, listh, labels, list->listLen);
		if(form->completeMode == 2){
		 	form->completeMode = 0;
		 	selection = 0;
		}else{
			form->completeMode = 1;
		}
		//return;
		if(selection != -1){
			int addLen = strlen(labels[selection])-token->length;
			printf("POS %d %d %d\n", form->realCaretPos.r, form->realCaretPos.c+1, addLen);
			addTrace(form->urStack, ADD, form->realCaretPos.r, form->realCaretPos.c+1, form->realCaretPos.r, form->realCaretPos.c+addLen, labels[selection]+token->length);
			addString(form->passage, labels[selection]+token->length, form->realCaretPos.r, form->realCaretPos.c+1);
			form->realCaretPos.c += addLen;
			form->caretPos.c += addLen;
			form->completeMode = 0;
		}
		for(i=0; i<list->listLen; i++){
			free(labels[i]);
			free(kthNode(list, i+1)->datptr);
		}
		free(list);
	}
}

void drawEditorSelection(EditorForm* form){

    // If selected range is empty or not in selection mode
    if(form->selectLeftPos.r == form->selectRightPos.r && form->selectLeftPos.c == form->selectRightPos.c) return;
    if(form->inSelectionMode == false) return;
    
    printf("Selected (%d, %d) - (%d, %d)\n", form->selectLeftPos.r, form->selectLeftPos.c, form->selectRightPos.r, form->selectRightPos.c);
    
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
    static char* menuListHelp[] = {"Help",
        "Open Mannual | Ctrl-H",
        "Change Theme | Ctrl-P", 
        "About | Ctrl-B"
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
    if (helperActivated()) menuListHelp[1] = "Close Mannual | Ctrl-H";
    else menuListHelp[1] = "Open Mannual | Ctrl-H";
    selection = menuList(GenUIID(0), x, y, w, wlist, h, menuListHelp, sizeof(menuListHelp) / sizeof(menuListHelp[0]));
    switch(selection) {
        case 1:
            if (helperActivated()) closeHelper();
			else activateHelper();
			break;
		case 2:
		    g_selection = 1 - g_selection;
		    break;
		case 3: if (aboutActivated()) closeAbout();
		    else activateAbout();
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
    SetPenColor(getColorByTokenType(token->type, token->level + 1));
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

