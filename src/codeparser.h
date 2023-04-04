#ifndef _CODE_PARSER_H_
#define _CODE_PARSER_H_

#include "bilinkedlist.h"

#define MAX_WORD_SIZE 40

//define token types
typedef enum{
	STRING,
	COMMENT,
	LEFT_PARENTHESES,
	RIGHT_PARENTHESES,
	LEFT_BRACKETS,
	RIGHT_BRACKETS,
	LEFT_BRACE,
	RIGHT_BRACE,
	SINGLE_QUOTE,
	DOUBLE_QUOTE,
	SPACE,
	OTHER
}tokenType;

//struct of a word
typedef struct{
	char str[MAX_WORD_SIZE];
	int strlen;
	tokenType type;
}word;

//struct of a line, each line is a linked list of words
typedef struct{
	linkedList lineList;
}line;

//struct of a passage, each passage is a linked list of lines
typedef struct{
	linkedList passList;
}passage;

void initPassage(passage *p);

//add a string at (row, col), unit = char
void addString(passage *p, char *str, int row, int col);

void inputString(char *str);

#endif
