#include <stdlib.h>

#include "fileio.h"
#include "textarea.h"
#include "codeparser.h"

void loadFile(Editor *editor, char *fileName){
	FILE *fp = fopen(fileName, "r");
	if(fp == NULL){
		puts("FAILED TO OPEN FILE");
		return;
	}
}

void saveFile(Editor *editor, char *fileName){
	FILE *fp = fopen(fileName, "w");
	if(fp == NULL){
		puts("FAILURE IN SAVE FILE");
		return;
	}
	
	EditorForm *form = editor->forms[editor->curSelect];
	Passage *p = form->passage;
	Listptr nowLineNode = kthNode(&(p->passList), 1);

 	while(nowLineNode != NULL){
 		Line* l = nowLineNode->datptr;
 		Listptr nowWordNode = kthNode(&(l->lineList), 1);
 		while(nowWordNode != NULL){
 			Token* w = nowWordNode->datptr;
 			fprintf(fp, "%s", w->content);
 			nowWordNode = nowWordNode->next;
 		}
 		nowLineNode = nowLineNode->next;
 	}
 	fclose(fp);
}
