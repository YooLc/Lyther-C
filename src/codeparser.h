#ifndef _CODE_PARSER_H_
#define _CODE_PARSER_H_

#include "doublylinkedlist.h"

#define MAX_WORD_SIZE 128
#define NEW(T) (T*)malloc(sizeof(T)) 

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
} TokenType;

//struct of a token
typedef struct{
	char content[MAX_WORD_SIZE];
	int length;
	TokenType type;
} Token;

//struct of a line, each line is a linked list of words
typedef struct{
	LinkedList lineList;
} Line;

//struct of a passage, each passage is a linked list of lines
typedef struct{
	LinkedList passList;
} Passage;

void initPassage(Passage *p);

//add a string at (row, col), unit = char
void addString(Passage *p, char *str, int row, int col);

void inputString(char *str);

//debug use
void printPassage(Passage *p);
#endif
