#ifndef _CODE_PARSER_H_
#define _CODE_PARSER_H_

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
	word* next;
	word* last;
}word;

//struct of a line, each line is a linked list of words
typedef struct{
	word* head;
	int wordlen;
	line* next;
	lint* last;
}line;

//struct of a passage, each passage is a linked list of lines
typedef struct{
	line *head;
	int headlen;
}passage;

#endif
