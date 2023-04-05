#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "codeparser.h"

/*
    Function: initPassage
    Initialize doubly linked list for a passage
*/
void initPassage(Passage *p){
	initList(&(p->passList));
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

static void setToken(Token* token, char *content, TokenType type){
	strcpy(token->content, content);
	token->length = strlen(content);
	token->type = type;
}

void parseLine(Passage *passage, int row){
	
	char tmpLine[MAX_LINE_SIZE], tmpWord[MAX_WORD_SIZE];
	int totLen = 0, idx = 1;

	Line  *line  = NEW(Line);
 	initList(&(line->lineList));
 	
 	//delete the old line, add a new empty line
 	totLen = getLine(passage, tmpLine, row);
 	deleteLine(passage, row);
 	addNode(&(passage->passList), row, line);
 	
	while(idx <= totLen){
		int cnt = 0;
		Token *token = NEW(Token);
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
				setToken(token, "'", SINGLE_QUOTE);
				idx++;
				break;
			case '"':
				setToken(token, "\"", DOUBLE_QUOTE);
				idx++;
				break;
			case ' ':
				setToken(token, " ", SPACE);
				idx++;
				break;
			case '/':
				
				if(idx < totLen && tmpLine[idx] == '/'){
					setToken(token, &tmpLine[idx-1], COMMENT);
					idx = totLen+1;
				}else{
					strncpy(token->content, &tmpLine[idx-1], 1);
					token->content[1] = '\0';
					token->type = OTHER;
					idx++;
				}
				break;
			default:
				if(!(tmpLine[idx-1] >= 'a' && tmpLine[idx-1] <= 'z' || tmpLine[idx-1] >= 'A' && tmpLine[idx-1] <= 'Z')){
					strncpy(token->content, &tmpLine[idx-1], 1);
					token->content[1] = '\0';
					token->type = OTHER;
					idx++;
					break;
				}
				while(idx <= totLen && (tmpLine[idx-1] >= 'a' && tmpLine[idx-1] <= 'z' || tmpLine[idx-1] >= 'A' && tmpLine[idx-1] <= 'Z')){
					tmpWord[cnt++] = tmpLine[idx-1];
					idx++;
				}
				tmpWord[cnt] = '\0';
				setToken(token, tmpWord, STRING);
		}
		printf("# %s\n", token->content);
		addNodeToTail(&(line->lineList), token);
	}
	
}

Listptr getPos(Passage *passage, int row, int col, int *offset){
	// Get the pointer of line in 'row'
	Line* l = kthNode(&(passage->passList), row)->datptr;
	
	Listptr nowNode = kthNode(&(l->lineList), 1);   // get the pointer of the first node of this line
	Token* token = nowNode->datptr;				    // pointer of the token in the node
	
	// Find the node which contains the 'col'th column
	int nowcol = token->length;
	while(nowcol < col - 1) {
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
void addString(Passage *passage, char *str, int row, int col) {
	int addLen = strlen(str), i=0;
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
	
	parseLine(passage, row);
}

//for debug use
void printPassage(Passage *p){
 	Listptr nowLineNode = kthNode(&(p->passList), 1);
 	printf("Total line num: %d\n", p->passList.listLen);
 	while(nowLineNode != NULL){
 		Line* l = nowLineNode->datptr;
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
