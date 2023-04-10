#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "codeparser.h"
#include "textarea.h"
/*
    Function: initPassage
    Initialize doubly linked list for a passage
*/
void initPassage(Passage *passage){
	initList(&(passage->passList));
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

static void setToken(Token* token, char *content, CodeTokenType type){
	strcpy(token->content, content);
	token->length = strlen(content);
	token->type = type;
}

static int isKeyWord(char *src){
	int i = 0;
	for(i = 0; i < 32; i++){
		if(strcmp(src, KeyWord[i]) == 0) return 1;
	}
	return 0;
}

int parseLine(Passage *passage, int row){
	
	char tmpLine[MAX_LINE_SIZE], tmpWord[MAX_WORD_SIZE];
	int totLen = 0, idx = 1, newLine = 0;

	Line  *line  = NEW(Line);
 	initList(&(line->lineList));
 	
 	//delete the old line, add a new empty line
 	totLen = getLine(passage, tmpLine, row);
 	deleteLine(passage, row);
 	addNode(&(passage->passList), row, line);
 	printf("Attempt to parse %s (length %d)", tmpLine, totLen);
 	
 	int length = totLen;
 	while (length > 0 && tmpLine[length - 1] == '\n') length--;
 	line->length = length;
 	
	while(idx <= totLen) {
		int cnt = 0;
		Token *token = NEW(Token);
		// token->selected = 1; // For test
		switch(tmpLine[idx-1]){
			case '(':
				setToken(token, "(", LEFT_PARENTHESES);
				idx++;
				break;
			case ')':
				setToken(token, ")", RIGHT_PARENTHESES);
				idx++;
				break;
			case '[':
				setToken(token, "[", LEFT_BRACKETS);
				idx++;
				break;
			case ']':
				setToken(token, "]", RIGHT_BRACKETS);
				idx++;
				break;
			case '{':
				setToken(token, "{", LEFT_BRACE);
				idx++;
				break;
			case '}':
				setToken(token, "}", RIGHT_BRACE);
				idx++;
				break;
			case '\'':
				idx++;
				tmpWord[cnt = 0] = '\'';
				while(idx <= totLen){
					tmpWord[++cnt] = tmpLine[idx-1];
					if(tmpLine[idx-1] == '\'') break;
					idx++;
				}
				idx++;
				tmpWord[cnt+1] = '\0';
				setToken(token, tmpWord, SINGLE_QUOTE);
				break;
			case '"':
				idx++;
				tmpWord[cnt = 0] = '"';
				while(idx <= totLen){
					tmpWord[++cnt] = tmpLine[idx-1];
					if(tmpLine[idx-1] == '"') break;
					idx++;
				}
				idx++;
				tmpWord[cnt+1] = '\0';
				setToken(token, tmpWord, DOUBLE_QUOTE);
				break;
			case ' ':
				setToken(token, " ", SPACE);
				idx++;
				break;
			case ';':
			    setToken(token, ";", SEMI_COLON);
			    idx++;
			    break;
			case '\n':
				setToken(token, "\n", ENTER);
			    idx++;
			    //if this is not the end of the line, creat a newline tag
				if(idx <= totLen) newLine = 1;
				break;
			case '/':
				if(idx < totLen && tmpLine[idx] == '/'){
					setToken(token, &tmpLine[idx-1], COMMENT);
					token->content[token->length-1] = '\0'; //deal with \n
					token->length--;
					idx = totLen;
				}else if(idx < totLen && tmpLine[idx] == '*'){
					setToken(token, "/*", LEFT_COMMENT);
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
					setToken(token, "*/", RIGHT_COMMENT);
					token->length = 2;
					idx += 2;
				}else{
					setToken(token, "*", RIGHT_COMMENT);
					token->length = 1;
					idx++;
				}
				break;
			case '#':
				setToken(token, &tmpLine[idx-1], PREPROCESS);
				token->content[token->length-1] = '\0'; //deal with \n
				token->length--;
				idx = totLen;
				break;
			default:
				if(!(tmpLine[idx-1] >= 'a' && tmpLine[idx-1] <= 'z' || tmpLine[idx-1] >= 'A' && tmpLine[idx-1] <= 'Z')){
					strncpy(token->content, &tmpLine[idx-1], 1);
					token->content[1] = '\0';
					token->type = OTHER;
					idx++;
					break;
				}
				cnt = 0;
				while(idx <= totLen && (tmpLine[idx-1] >= 'a' && tmpLine[idx-1] <= 'z' || tmpLine[idx-1] >= 'A' && tmpLine[idx-1] <= 'Z')){
					tmpWord[cnt++] = tmpLine[idx-1];
					idx++;
				}
				tmpWord[cnt] = '\0';
				setToken(token, tmpWord, STRING);
		}
		token->length = strlen(token->content);
             
        if(isKeyWord(token->content)) token->type = KEYWORD;
		// printf("# %s\n", token->content);
		// if (token->type != ENTER)
            addNodeToTail(&(line->lineList), token);
		//If there is \n more than one, add a new line
		if(newLine) {
			line = NEW(Line);
 			initList(&(line->lineList));
 			addNode(&(passage->passList), ++row, line);
 			newLine = 0;
		}
	}
	
	return row;
}

Listptr getPos(Passage *passage, int row, int col, int *offset){
	// Get the pointer of line in 'row'
	Line* l = kthNode(&(passage->passList), row)->datptr;
	
	Listptr nowNode = kthNode(&(l->lineList), 1);   // get the pointer of the first node of this line
	Token* token = nowNode->datptr;                 // pointer of the token in the node
	
	// Find the node which contains the 'col'th column
	int nowcol = token->length;
	while(nowcol < col) {
		nowNode = nowNode->next;
		token = nowNode->datptr;
		nowcol += token->length;
	}
	
	// Calculate the exact col in this word
	int nowLen = token->length;
	*offset = nowLen - (nowcol - col + 1);
	
	return nowNode;
}
/*
    Function: addString(Passage *p, char *str, int row, int col)
    Insert string into the passage at given position, 
    which is represented by row and col index.
    If give row is not in the passage, then create a new row in the list.
*/ 
PosRC addString(Passage *passage, char *str, int row, int col) {
	int addLen = strlen(str), i = 0;
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
		parseLine(passage, row);
		return;
	}

	int offset = 0;
	Token* token = getPos(passage, row, col, &offset)->datptr;
	
	char tmpstr[MAX_WORD_SIZE] = "";
	// tmpstr = orginal(before offset) + str + original(after offset)
	strncpy(tmpstr, token->content, offset);
	strcat(tmpstr, str);
	strcat(tmpstr, token->content + offset);
	
	strcpy(token->content, tmpstr);
	token->length = strlen(token->content);
	
	//calculate the cursor position, a naive algorithm
	int newRow = parseLine(passage, row);
	
	getLine(&(passage->passList), tmpstr, newRow);
	
	int nowCol = newRow==row? col : 1;
	
	char lastChar = str[strlen(str) - 1];
	while(tmpstr[col] != lastChar) col++;
	
	PosRC posRC = {newRow, col};
	
	return posRC;
}

void deleteString(Passage *passage, int rows, int cols, int rowt, int colt){
	int i = 0;
	
	char tmpLine1[MAX_LINE_SIZE], tmpLine2[MAX_LINE_SIZE];    //store string in the first and last row
	
	getLine(passage, tmpLine1, rows);
	getLine(passage, tmpLine2, rowt);
	
	//store the remaining string in tmpLine1
	tmpLine1[cols - 1] = '\0';
	strcat(tmpLine1, tmpLine2 + colt);
	
	//delete original string
	for(i = rowt; i >= rows; i--){
		deleteLine(&(passage->passList), i);	
	}
	
	//initialize and insert the remaining string
	Line  *line  = NEW(Line);
	Token *token = NEW(Token);
	strcpy(token->content, tmpLine1);
	token->length = strlen(tmpLine1);
	token->type = STRING;
	initList(&(line->lineList));
	addNodeToTail(&(line->lineList), token);
	addNode(&(passage->passList), rows, line);

	parseLine(passage, rows);
}

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

//for debug use
void printPassage(Passage *p){
 	Listptr nowLineNode = kthNode(&(p->passList), 1);
 	printf("Total line num: %d\n", p->passList.listLen);
 	while(nowLineNode != NULL){
 		Line* l = nowLineNode->datptr;
 		printf("    Line length: %d\n", l->length);
 		printf("    Total Node num: %d\n", l->lineList.listLen);
 		Listptr nowWordNode = kthNode(&(l->lineList), 1);
 		while(nowWordNode != NULL){
 			Token* w = nowWordNode->datptr;
 			printf("		content: \"%s\", len: %d, type: %d\n", w->content, w->length, w->type);
 			nowWordNode = nowWordNode->next;
 		}
 		nowLineNode = nowLineNode->next;
 	}
 }
