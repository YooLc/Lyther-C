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

extern double winWidth, winHeight;
extern int textPointSize;
extern double textFontHeight, indexLength;
extern UIState gs_UIState;

static bool isControlDown = 0;

void moveCaret(EditorForm *form, CaretAction action, char* curLine, char* preLine) {
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

void deleteLastChar(EditorForm* form) {
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

PosRC pixelToPosRC(EditorForm *form, int px, int py) {
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
    //static bool completed = false;
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
//        switch(ch) {
//            case '}': case ']': case ')':
//            case '>': case '\"': case '\'':
//                if (completed) {
//                    completed = false;
//                    return;
//                }
//        }
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
            //completed = true;
        } //else completed = false;
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
    } else if ((ch == '\n' || ch == '\r') && form->completeMode == 0) {
        int offset;
        Token* lastToken = getPos(form->passage, curPos.r, curPos.c + 1, &offset)->datptr;
        printf("Caught return key, attempt to complete!\n");
        int i;
        for (i = 0; i < lastToken->level * INDENT_LENGTH; i++) {
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
            	if(curForm->completeMode == 1){
					curForm->completeMode = 2;
				}else{
                	addTrace(curForm->urStack, ADD, curPos.r, curPos.c+1, curPos.r, curPos.c+1, "\n");
                	curForm->caretPos = curForm->realCaretPos = addString(curForm->passage, "\n", curPos.r, curPos.c + 1);
            	}
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
