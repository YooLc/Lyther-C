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
