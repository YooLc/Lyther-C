#ifndef _CODE_PARSER_H_
#define _CODE_PARSER_H_

#include "doublylinkedlist.h"

#define MAX_WORD_SIZE 128
#define NEW(T) (T*)malloc(sizeof(T)) 

/*
    Enum: TokenType
    Type of token parsed from code parser
    Currently they are STRING by default
*/
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

/*
    Struct: Token
    Smallest unit of the whole code passage, 
    parsed from code parser
*/
typedef struct{
	char content[MAX_WORD_SIZE];
	int length;
	TokenType type;
} Token;

/*
    Struct: Line
    Each line is a doubly linked list of tokens
*/
typedef struct{
	LinkedList lineList;
} Line;

/*
    Struct: Passage
    Each passage is a doubly linked list of lines
*/
typedef struct{
	LinkedList passList;
} Passage;

/*
    Function: initPassage
    Initialize doubly linked list for a passage
*/
void initPassage(Passage *p);

/*
    Function: addString(Passage *p, char *str, int row, int col)
    Insert string into the passage at given position, 
    which is represented by row and col index.
    If give row is not in the passage, then create a new row in the list.
*/ 
void addString(Passage *passage, char *str, int row, int col);

void inputString(char *str);

// debug use
void printPassage(Passage *p);
#endif
