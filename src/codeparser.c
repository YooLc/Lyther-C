#include <string.h>
#include <stdlib.h>

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
	if(p->passList.listLen == row-1){
		Line  *line  = NEW(Line);
		Token *token = NEW(Token);
		strcpy(token->content, str);
		token->length = addLen;
		token->type = STRING;
		addNodeToTail(&line->lineList), w);
		addNodeToTail(&(passage->passList), l);
		return;
	}
	
	// Get the pointer of line in 'row'
	Line* l = kthNode(&(p->passList), row)->datptr;
	
	Listptr nowNode = kthNode(&(l->lineList), 1);   // get the pointer of the first node of this line
	Token* token = nowNode->datptr;				    // pointer of the token in the node
	
	// Find the node which contains the 'col'th column
	int nowcol = token->length;
	while(nowcol < col) {
		nowNode = nowNode->next;
		token = nowNode->datptr;
		nowcol += w->strlen;
	}
	
	// Calculate the exact col in this word
	int nowLen = w->strlen;
	int offset = nowLen - (nowcol - col);
	
	char tmpstr[MAX_WORD_SIZE];
	//first part of the origin string
	for(i = 0; i < offset; i++){
		tmpstr[i] = w->str[i];
	}
	//added string
	strcat(tmpstr, str);
	//second part of the origin string
	for(i=offset; i<nowLen; i++){
		tmpstr[i+addLen] = w->str[i];
	}
	//store the new string in the word
	strcpy(w->str, tmpstr);	
}
