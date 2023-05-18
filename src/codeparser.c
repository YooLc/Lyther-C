#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#include "codeparser.h"
#include "textarea.h"
#include "trie.h"

/*
    Function: initPassage
    Initialize doubly linked list for a passage
*/
void initPassage(Passage *passage){
    initList(&(passage->passList));
    initTrie(&(passage->trie));
}

void deleteLine(Passage *passage, int row){
    Line* l = kthNode(&(passage->passList), row)->datptr;
    Listptr nowWordNode = kthNode(&(l->lineList), 1);
    
    while(nowWordNode->next != NULL){
         deleteNode(&(l->lineList), 2);
     }
     deleteNode(&(l->lineList), 1);
     
     deleteNode(&(passage->passList), row);
}

int getLine(Passage *passage, char *dst, int row){
    int totLen = 0;
    strcpy(dst, "");
    
    if(passage->passList.listLen == 0) return 0;
    
    Line* l = kthNode(&(passage->passList), row)->datptr;
    Listptr nowWordNode = kthNode(&(l->lineList), 1);
    
    while(nowWordNode != NULL){
         Token* token = nowWordNode->datptr;
         strcat(dst, token->content);
         nowWordNode = nowWordNode->next;
         totLen += token->length;
     }
     
     return totLen;
}

static void setToken(Token* token, char *content, int level, CodeTokenType type){
    strcpy(token->content, content);
    token->length = strlen(content);
    token->level = level;
    token->type = type;
}

static int isKeyWord(char *src){
    int i = 0;
    for(i = 0; i < 32; i++){
        if(strcmp(src, KeyWord[i]) == 0) return 1;
    }
    return 0;
}

static void maintainLineLength(Line *line){
    if(line->lineList.listLen == 0){
        line->length = 0;
        return;
    }
    
    line->length = 0;
    
    Listptr nowNode = kthNode(&(line->lineList), 1);
    Token *token = nowNode->datptr;
    while(1){
        if(token->type == ENTER && nowNode->next == NULL) break;
        line->length += token->length;
        if(nowNode->next == NULL) break;
        nowNode = nowNode->next;
        token = nowNode->datptr;
    }
}

static int isCharacter(char ch){
    if(ch >= 'a' && ch <= 'z') return 1;
    if(ch >= 'A' && ch <= 'Z') return 1;
    if(ch < 0) return 1;    //Chinese character
    return 0;
}

int parseLine(Passage *passage, int row){
    
    char tmpLine[MAX_LINE_SIZE], tmpWord[MAX_WORD_SIZE];
    int totLen = 0, idx = 1, newLine = 0;
    
	int levelCounter = 0, offset;
	if (row > 1) {
	    Line*  lastLine = kthNode(passage, row - 1)->datptr;
	    Token* lastToken = getPos(passage, row - 1, lastLine->length + 1, &offset)->datptr;
	    levelCounter = lastToken->level;
    }
    int initLevel = levelCounter;

    Line  *line  = NEW(Line);
    initList(&(line->lineList));
     
    //store the target line in char array <tmpLine>
     totLen = getLine(passage, tmpLine, row);
     
     //delete the old line, add a new empty line
     deleteLine(passage, row);
     addNode(&(passage->passList), row, line);
     printf("Attempt to parse %s (length %d)", tmpLine, totLen);
     
    while(idx <= totLen) {
        int cnt = 0;
        Token *token = NEW(Token);
        // token->selected = 1; // For test
        switch(tmpLine[idx-1]){
            case '(':
                setToken(token, "(", levelCounter, LEFT_PARENTHESES);
                levelCounter++;
                idx++;
                break;
            case ')':
                levelCounter = max(0, levelCounter - 1);
                setToken(token, ")", levelCounter, RIGHT_PARENTHESES);
                idx++;
                break;
            case '[':
                setToken(token, "[", levelCounter, LEFT_BRACKETS);
                levelCounter++;
                idx++;
                break;
            case ']':
                levelCounter = max(0, levelCounter - 1);
                setToken(token, "]", levelCounter, RIGHT_BRACKETS);
                idx++;
                break;
            case '{':
                setToken(token, "{", levelCounter, LEFT_BRACE);
                levelCounter++;
                idx++;
                break;
            case '}':
                levelCounter = max(0, levelCounter - 1);
                setToken(token, "}", levelCounter, RIGHT_BRACE);
                idx++;
                break;
            case '\'':
                idx++;
                tmpWord[cnt = 0] = '\'';
                while(idx <= totLen){
                    tmpWord[++cnt] = tmpLine[idx-1];
                    if(tmpLine[idx-1] == '\'' || tmpLine[idx-1] == '\n') break;
                    idx++;
                }
                if(tmpWord[cnt] != '\n') idx++;
                if(tmpWord[cnt] == '\n') cnt--;
                tmpWord[cnt+1] = '\0';
                setToken(token, tmpWord, levelCounter, SINGLE_QUOTE);
                break;
            case '"':
                idx++;
                tmpWord[cnt = 0] = '"';
                while(idx <= totLen){
                    tmpWord[++cnt] = tmpLine[idx-1];
                    if(tmpLine[idx-1] == '"' || tmpLine[idx-1] == '\n') break;
                    idx++;
                }
                if(tmpWord[cnt] != '\n') idx++;
                if(tmpWord[cnt] == '\n') cnt--;
                tmpWord[cnt+1] = '\0';
                setToken(token, tmpWord, levelCounter, DOUBLE_QUOTE);
                break;
            case ' ':
                setToken(token, " ", levelCounter, SPACE);
                idx++;
                break;
            case ';':
                setToken(token, ";", levelCounter, SEMI_COLON);
                idx++;
                break;
            case '\n':
                setToken(token, "\n", levelCounter, ENTER);
                idx++;
                //if this is not the end of the line, creat a newline tag
                if(idx <= totLen) newLine = 1;
                break;
            case '/':
                if(idx < totLen && tmpLine[idx] == '/'){
                    cnt = -1;
                    while(idx <= totLen){
                        if(tmpLine[idx-1] == '\n') break;
                        tmpWord[++cnt] = tmpLine[idx-1];
                        idx++;
                    }
                    tmpWord[cnt+1] = '\0';
                    setToken(token, tmpWord, levelCounter, COMMENT);
                }else if(idx < totLen && tmpLine[idx] == '*'){
                    setToken(token, "/*", levelCounter, LEFT_COMMENT);
                    token->length = 2;
                    idx += 2;
                }else{
                    strncpy(token->content, &tmpLine[idx-1], 1);
                    token->content[1] = '\0';
                    token->type = OTHER;
                    idx++;
                }
                break;
            case '*':
                if(idx < totLen && tmpLine[idx] == '/'){
                    setToken(token, "*/", levelCounter, RIGHT_COMMENT);
                    token->length = 2;
                    idx += 2;
                }else{
                    setToken(token, "*", levelCounter, RIGHT_COMMENT);
                    token->length = 1;
                    idx++;
                }
                break;
            case '#':
                cnt = -1;
                while(idx <= totLen){
                    if(tmpLine[idx-1] == '\n') break;
                    tmpWord[++cnt] = tmpLine[idx-1];
                    idx++;
                }
                tmpWord[cnt+1] = '\0';
                setToken(token, tmpWord, levelCounter, PREPROCESS);
                break;
            default:
                if( !isCharacter(tmpLine[idx-1]) ){
                    strncpy(token->content, &tmpLine[idx-1], 1);
                    token->content[1] = '\0';
                    token->type = OTHER;
                    idx++;
                    break;
                }
                cnt = 0;
                while(idx <= totLen && isCharacter(tmpLine[idx-1]) ){
                    tmpWord[cnt++] = tmpLine[idx-1];
                    idx++;
                }
                tmpWord[cnt] = '\0';
                setToken(token, tmpWord, levelCounter, STRING);
        }
        
        token->length = strlen(token->content);
             
        if(isKeyWord(token->content)) token->type = KEYWORD;

        addNodeToTail(&(line->lineList), token);    //add this token to the end of the line
        
        //If there is \n more than one, add a new line
        if(newLine) {
            maintainLineLength(line);    //maintain the length of the old line
            line = NEW(Line);
             initList(&(line->lineList));
             addNode(&(passage->passList), ++row, line);
             newLine = 0;
        }
        
    }
    
    maintainLineLength(line); //maintain the length of the current line
    if (levelCounter != initLevel)
        maintainLevel(passage, row, line->length);
    return row;
}

Listptr getPos(Passage *passage, int row, int col, int *offset){
    // Get the pointer of line in 'row'
    Line* l = kthNode(&(passage->passList), row)->datptr;
    
    Listptr nowNode = kthNode(&(l->lineList), 1);   // get the pointer of the first node of this line
    Token* token = nowNode->datptr;                 // pointer of the token in the node
//    printf("Getting pos, current token [%s]\n", token->content);
    // Find the node which contains the 'col'th column
    int nowcol = token->length;
    while(nowcol < col) {
        nowNode = nowNode->next;
        token = nowNode->datptr;
        nowcol += token->length;
    }
    
    int nowLen = token->length;
    *offset = nowLen - (nowcol - col + 1);
    
    return nowNode;
}

static void refreshTrieAdd(Passage *passage, int rows, int rowt){
	int nowRow = rows, tokenIndex = 1;
	Line *line = NULL;
	Token *token = NULL;
	
	for(nowRow = rows; nowRow <= rowt; nowRow++){
		line = kthNode(&(passage->passList), nowRow)->datptr;
		
		for(tokenIndex = 1; tokenIndex <= line->lineList.listLen; tokenIndex++){
			token = kthNode(&(line->lineList), tokenIndex)->datptr;
			if(token->type == STRING){
				printf("STR : %s %d", token->content, token->length);
				token->content[token->length] = '\0';
				addStringToTrie(passage->trie.root, token->content);
			} 
		}
		
	}
	//traverseTree(passage->trie.root);
	printAllString(passage->trie.root);
}

static void refreshTrieDelete(Passage *passage, int rows, int rowt){
	int nowRow = rows, tokenIndex = 1;
	Line *line = NULL;
	Token *token = NULL;
	printf("%d %d %d\n", rows, rowt, passage->passList.listLen);
	//return;
	for(nowRow = rows; nowRow <= rowt; nowRow++){
		
		line = kthNode(&(passage->passList), nowRow)->datptr;
		
		for(tokenIndex = 1; tokenIndex < line->lineList.listLen; tokenIndex++){
			token = kthNode(&(line->lineList), tokenIndex)->datptr;
			if(token->type == STRING) deleteStringInTrie(passage->trie.root, token->content);
		}
		
	}
	//traverseTree(passage->trie.root);
	//printAllString(passage->trie.root);
}
/*
    Function: addString(Passage *p, char *str, int row, int col)
    Insert string into the passage at given position, 
    which is represented by row and col index.
    If give row is not in the passage, then create a new row in the list.
*/ 
PosRC addString(Passage *passage, char *str, int row, int col) {
    int addLen = strlen(str), i = 0;
    PosRC posRC;
    // If it is a new row
    if(passage->passList.listLen == row - 1) {
        Line  *line  = NEW(Line);
        Token *token = NEW(Token);
        strcpy(token->content, str);
        token->length = addLen;
        token->type = STRING;
        initList(&(line->lineList));
        addNodeToTail(&(line->lineList), token);
        addNodeToTail(&(passage->passList), line);
        
        //calculate the cursor position
        posRC.r = parseLine(passage, row);
        line = kthNode(&(passage->passList), posRC.r)->datptr;    //get the last row
        posRC.c = line->length;
		refreshTrieAdd(passage, row, posRC.r);
        return posRC;
    }

    int offset = 0, nodeIndex = 0;
    Line *line = kthNode(&(passage->passList), row)->datptr;
    Token *token = getPos(passage, row, col, &offset)->datptr;
    Token *nextToken = NEW(Token);
    
    refreshTrieDelete(passage, row, row);
    nodeIndex = getNodeIndex(&(line->lineList), getPos(passage, row, col, &offset));
    printf("OFFSET = %d\n, NODEIDEX = %d CONTENT = %s\n", offset, nodeIndex,token->content);
    
    strcpy(nextToken->content, token->content + offset);
    nextToken->length = strlen(nextToken->content);
    
    if(offset == 0){
		deleteNode(&(line->lineList), nodeIndex);
    	nodeIndex--;
	}else{
		token->content[offset] = '\0';
		token->length = strlen(token->content);
	}
    
    while(strlen(str) > MAX_WORD_SIZE){
		Token *newToken = NEW(Token);
		strncpy(newToken->content, str, MAX_WORD_SIZE);
		newToken->content[MAX_WORD_SIZE] = '\0';
		newToken->length = MAX_WORD_SIZE;
		addNode(&(line->lineList), ++nodeIndex, newToken);
		str += MAX_WORD_SIZE;
	}
	
	if(strlen(str) > 0){
		Token *newToken = NEW(Token);
		strcpy(newToken->content, str);
		newToken->length = strlen(newToken->content);
		addNode(&(line->lineList), ++nodeIndex, newToken);
	}
	
	addNode(&(line->lineList), ++nodeIndex, nextToken);
	/*
    strcat(tmpstr, str);
    strcat(tmpstr, token->content + offset);
    
    strcpy(token->content, tmpstr);
    token->length = strlen(token->content);
    */
    //calculate the cursor position, a naive algorithm
    int newRow = parseLine(passage, row);
    char tmpstr[MAX_LINE_SIZE];
    getLine(&(passage->passList), tmpstr, newRow);
    
    int nowCol = (newRow == row) ? col : 0;
    
    char lastChar = str[strlen(str) - 1];
    if (lastChar != '\n')
        while(tmpstr[nowCol-1] != lastChar) 
            nowCol++;
    
    posRC.r = newRow;
    posRC.c = nowCol;
	refreshTrieAdd(passage, row, newRow);
    return posRC;
}

void deleteString(Passage *passage, int rows, int cols, int rowt, int colt){
    int i = 0;

    char tmpLine1[MAX_LINE_SIZE], tmpLine2[MAX_LINE_SIZE];    //store string in the first and last row
    char prevLine[MAX_LINE_SIZE], nextLine[MAX_LINE_SIZE];
    char targetLine[4*MAX_LINE_SIZE] = "\0";
    
    if(rows > rowt || (rows == rowt && cols > colt)){
        i = rows;
        rows = rowt;
        rowt = i;
        i = cols;
        cols = colt;
        colt = i;
    }
    refreshTrieDelete(passage, rows, rowt);
    if(rows > 1){
		getLine(passage, prevLine, rows-1);
		refreshTrieDelete(passage, rows-1, rows-1);
	} 
    if(rowt < passage->passList.listLen){
		refreshTrieDelete(passage, rowt+1, rowt+1);
		getLine(passage, nextLine, rowt+1);
	}
    getLine(passage, tmpLine1, rows);
    getLine(passage, tmpLine2, rowt);
    
    // If it is GB2312 character, then delete two characters.
    if(rows > 1) strcat(targetLine, prevLine);
    //if (tmpLine1[cols - 1] < 0 && cols > 1) 
    //    strncat(targetLine, tmpLine1, cols - 2);
    //else
        strncat(targetLine, tmpLine1, cols - 1);
    strcat(targetLine, tmpLine2 + colt);
    if(rowt < passage->passList.listLen) strcat(targetLine, nextLine);

    printf("\n\n **** DELETE STRING : \"%s\"\n", targetLine);
    if(rowt < passage->passList.listLen) deleteLine(&(passage->passList), rowt+1);
    //delete original string
    for(i = rowt; i >= rows; i--){
        deleteLine(&(passage->passList), i);    
    }
    
    if(rows > 1) deleteLine(&(passage->passList), rows-1);
    //initialize and insert the remaining string
    Line  *line  = NEW(Line);
    Token *token = NEW(Token);
    strcpy(token->content, targetLine);
    token->length = strlen(targetLine);
    token->type = STRING;
    initList(&(line->lineList));
    addNodeToTail(&(line->lineList), token);
    addNode(&(passage->passList), (rows>1)?(rows-1):(rows), line);

    int newRow = parseLine(passage, (rows>1)?(rows-1):rows);
    refreshTrieAdd(passage, (rows>1)?(rows-1):rows, newRow);
    printf("ROW : %d\n", newRow);
}
/*
void cancelNewline(Passage *passage, int row) {
    // Validation check
    if (row <= 1 || row > (passage->passList).listLen) return;
    
    // Add current line to previous line, then delete current line
    char preLine[MAX_LINE_SIZE], curLine[MAX_LINE_SIZE];
    getLine(passage, preLine, row - 1);
    getLine(passage, curLine, row);
    
    int idx = strlen(preLine) - 1;
    while (preLine[idx] == '\n') preLine[idx] = '\0';
    strcat(preLine, curLine);
    
    deleteLine(&(passage->passList), row - 1);
    deleteLine(&(passage->passList), row - 1); // Note that prev line has deleted
    
    //initialize and insert the remaining string
    Line  *line  = NEW(Line);
    Token *token = NEW(Token);
    strcpy(token->content, preLine);
    token->length = strlen(preLine);
    token->type = STRING;
    initList(&(line->lineList));
    addNodeToTail(&(line->lineList), token);
    addNode(&(passage->passList), row - 1, line);

    parseLine(passage, row - 1);
}
*/

char *getString(Passage *passage, int rows, int cols, int rowt, int colt){
    char *dst = (char*)malloc(sizeof(char)*MAX_LINE_SIZE*(rowt-rows+1));
    char tmpLine[MAX_LINE_SIZE], targetLine[MAX_LINE_SIZE];
    int nowRow = rows;
    dst[0] = '\0';
    
    while(nowRow <= rowt){
        getLine(passage, tmpLine, nowRow);
        strcpy(targetLine, tmpLine);
        if(rows == rowt){
            strncpy(targetLine, tmpLine+cols-1, colt-cols+1);
            targetLine[colt-cols+1] = '\0';
        }else if(nowRow == rows){
            strcpy(targetLine, tmpLine+cols-1);
        }else if(nowRow == rowt){
            strncpy(targetLine, tmpLine, colt);
            targetLine[colt] = '\0';
        }
        nowRow++;
        strcat(dst, targetLine);
        printf("Target content \"%s\"",targetLine);
    }
    printf("Range content \"%s\"",dst);
    return dst;
}

PosRC searchForwardByChar(Passage *passage, int row, int col, char ch){
    //Initialize variables
    int offset = 0, i = 0, len = 0;
    PosRC posRC = {-1, -1};
    char tmpLine1[MAX_LINE_SIZE] = "";
    Listptr nowNode = getPos(passage, row, col, &offset);
    Token *token = nowNode->datptr;

    //attempt to find <ch> in the first row
    getLine(passage, tmpLine1, row);
    len = strlen(tmpLine1);
    for(i=col-1; i<len; i++){
        if(tmpLine1[i] == ch){
            posRC.r = row;
            posRC.c = i+1;
            return posRC;
        }
    }
    
    //attempt to find <ch> in the latter rows
    while(++row <= passage->passList.listLen){
        getLine(passage, tmpLine1, row);
        len = strlen(tmpLine1);
        for(i=0; i<len; i++){
            if(tmpLine1[i] == ch){
                posRC.r = row;
                posRC.c = i+1;
                return posRC;
            }
        }
    }
    
    return posRC;
}

void maintainLevel(Passage *passage, int row, int col){
    int offset, levelCounter;
    ListNode* nowNode = getPos(passage, row, col, &offset);
    Token* token = nowNode->datptr;
    levelCounter = token->level;
    
    if (token->type == LEFT_PARENTHESES || token->type == LEFT_BRACKETS || token->type == LEFT_BRACE)
        levelCounter++;
    
    while(levelCounter >= 0) {
        nowNode = nowNode->next;
        if (nowNode == NULL) {
            if (row < passage->passList.listLen) {
                nowNode = getPos(passage, ++row, 0, &offset);
            } else return; // EOF
        }
        token = nowNode->datptr;
        switch(token->type) {
            case LEFT_PARENTHESES: case LEFT_BRACKETS: case LEFT_BRACE:
                token->level = levelCounter;
                levelCounter++;
                break;
            case RIGHT_PARENTHESES: case RIGHT_BRACKETS: case RIGHT_BRACE:
                levelCounter--;
                if (levelCounter < 0) return;
                token->level = levelCounter;
                break;
            default:
                token->level = levelCounter;
                break;
        }
    }
}

PosRC searchBackwardByChar(Passage *passage, int row, int col, char ch){
    //Initialize variables
    int offset = 0, i = 0, len = 0;
    PosRC posRC = {-1, -1};
    char tmpLine1[MAX_LINE_SIZE] = "";
    Listptr nowNode = getPos(passage, row, col, &offset);
    Token *token = nowNode->datptr;

    //attempt to find <ch> in the first row
    getLine(passage, tmpLine1, row);
    for(i=col-1; i>=0; i--){
        if(tmpLine1[i] == ch){
            posRC.r = row;
            posRC.c = i+1;
            return posRC;
        }
    }
    
    //attempt to find <ch> in the previous rows
    while(--row > 0){
        getLine(passage, tmpLine1, row);
        len = strlen(tmpLine1);
        for(i=len-1; i>=0; i--){
            if(tmpLine1[i] == ch){
                posRC.r = row;
                posRC.c = i+1;
                return posRC;
            }
        }
    }
    
    return posRC;
}

//for debug use
void printPassage(Passage *p){ 
    return;
     Listptr nowLineNode = kthNode(&(p->passList), 1);
     printf("Total line num: %d\n", p->passList.listLen);
     while(nowLineNode != NULL){
         Line* l = nowLineNode->datptr;
         printf("    Line length: %d\n", l->length);
         printf("    Total Node num: %d\n", l->lineList.listLen);
         Listptr nowWordNode = kthNode(&(l->lineList), 1);
         while(nowWordNode != NULL){
             Token* w = nowWordNode->datptr;
             printf("        content: \"%s\", len: %d, type: %d, level: %d\n", w->content, w->length, w->type, w->level);
             nowWordNode = nowWordNode->next;
         }
         nowLineNode = nowLineNode->next;
     }
 }
