#include "codeparser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void initPassage(passage *p){
	initList(&(p->passList));
}

void addString(passage *p, char *str, int row, int col){
	int addLen = strlen(str), i=0;
	//If it is a new row

	if(p->passList.listLen == row-1){
		line *l = (line*)malloc(sizeof(line));
		initList(&(l->lineList));
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
	while(nowcol < col-1){
		nowNode = nowNode->next;
		w = nowNode->datptr;
		nowcol += w->strlen;
	}
	
	//calculate the exact col in this word
	int nowLen = w->strlen;
	int offset = nowLen - (nowcol-col+1);

	char tmpstr[MAX_WORD_SIZE];
	
	//first part of the origin string
	for(i=0; i<offset; i++){
		tmpstr[i] = w->str[i];
	}
	tmpstr[i] = '\0';

	//added string
	strcat(tmpstr, str);

	//second part of the origin string
	for(i=offset; i<nowLen; i++){
		tmpstr[i+addLen] = w->str[i];
	}
	tmpstr[i+addLen] = '\0';

	//store the new string in the word
	strcpy(w->str, tmpstr);
	w->strlen += addLen;
}

//for debug use
void printPassage(passage *p){
	listptr nowLineNode = kthNode(&(p->passList), 1);
	printf("Line num: %d\n", p->passList.listLen);
	while(nowLineNode != NULL){
		line* l = nowLineNode->datptr;
		printf("    Node num: %d\n", l->lineList.listLen);
		listptr nowWordNode = kthNode(&(l->lineList), 1);
		while(nowWordNode != NULL){
			word* w = nowWordNode->datptr;
			printf("content: %s, len: %d, type: %d\n", w->str, w->strlen, w->type);
			nowWordNode = nowWordNode->next;
		}
		nowLineNode = nowLineNode->next;
	}
}
