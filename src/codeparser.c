#include "codeparser.h"
#include <string.h>
#include <stdlib.h>

void initPassage(passage *p){
	initList(&(p->passList));
}
//4:15
void addString(passage *p, char *str, int row, int col){
	int addLen = strlen(str), i=0;
	//If it is a new row
	if(p->passList.listLen == row-1){
		line *l = (line*)malloc(sizeof(line));
		word *w = (word*)malloc(sizeof(word));
		strcpy(w->str, str);
		w->strlen = addLen;
		w->type = STRING;
		addNodeToTail(&(l->lineList), w);
		addNodeToTail(&(p->passList), l);
		return;
	}
	
	//get the pointer of line in 'row'
	line* l = kthNode(&(p->passList), row)->datptr;
	
	
	listptr nowNode = kthNode(&(l->lineList), 1);   //get the pointer of the first node of this line
	word* w = nowNode->datptr;						//pointer of the word in the node
	
	//find the node which contains the 'col'th column
	int nowcol = w->strlen;
	while(nowcol < col){
		nowNode = nowNode->next;
		w = nowNode->datptr;
		nowcol += w->strlen;
	}
	
	//calculate the exact col in this word
	int nowLen = w->strlen;
	int offset = nowLen - (nowcol-col);
	
	char tmpstr[MAX_WORD_SIZE];
	//first part of the origin string
	for(i=0; i<offset; i++){
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
