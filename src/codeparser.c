#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "codeparser.h"

void initPassage(Passage *p){
	initList(&(p->passList));
}
/*
    Function: addString(Passage *p, char *str, int row, int col)
    Insert string into the passage at given position, 
    which is represented by row and col index.
    If give row is not in the passage, then create a new row in the list.
*/ 
void addString(Passage *passage, char *str, int row, int col){
	int addLen = strlen(str), i=0;
	// If it is a new row
	if(passage->passList.listLen == row-1) {
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
	int offset = nowLen - (nowcol - col + 1);
	
	char tmpstr[MAX_WORD_SIZE];
	
	//first part of the origin string
	for(i = 0; i < offset; i++){
		tmpstr[i] = token->content[i];
	}
	tmpstr[i] = '\0';

	//added string
	strcat(tmpstr, str);

	//second part of the origin string
	for(i = offset; i < nowLen; i++){
		tmpstr[i + addLen] = token->content[i];
	}
	tmpstr[i + addLen] = '\0';

	//store the new string in the word
	strcpy(token->content, tmpstr);
	token->length += addLen;
}

//for debug use
// void printPassage(passage *p){
// 	listptr nowLineNode = kthNode(&(p->passList), 1);
// 	printf("Line num: %d\n", p->passList.listLen);
// 	while(nowLineNode != NULL){
// 		line* l = nowLineNode->datptr;
// 		printf("    Node num: %d\n", l->lineList.listLen);
// 		listptr nowWordNode = kthNode(&(l->lineList), 1);
// 		while(nowWordNode != NULL){
// 			word* w = nowWordNode->datptr;
// 			printf("content: %s, len: %d, type: %d\n", w->str, w->strlen, w->type);
// 			nowWordNode = nowWordNode->next;
// 		}
// 		nowLineNode = nowLineNode->next;
// 	}
// }
