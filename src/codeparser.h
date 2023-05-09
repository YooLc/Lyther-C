#ifndef _CODE_PARSER_H_
#define _CODE_PARSER_H_

#include "doublylinkedlist.h"
#define MAX_WORD_SIZE 128
#define MAX_LINE_SIZE 2048
#define NEW(T) (T*)malloc(sizeof(T)) 
#define LOG(x,y) printf("[LOG][%d -> %s()] ", __LINE__, __func__); \
                 printf(x,y)

/*
    Enum: TokenType
    Type of token parsed from code parser
    Currently they are STRING by default
*/
typedef enum{
    STRING,
    COMMENT,
    LEFT_COMMENT,
    RIGHT_COMMENT,
    PREPROCESS,
    KEYWORD,
    LEFT_PARENTHESES,
    RIGHT_PARENTHESES,
    LEFT_BRACKETS,
    RIGHT_BRACKETS,
    LEFT_BRACE,
    RIGHT_BRACE,
    SINGLE_QUOTE,
    DOUBLE_QUOTE,
    SEMI_COLON,
    ENTER,
    SPACE,
    OTHER
} CodeTokenType;

/*
    Struct: Token
    Smallest unit of the whole code passage, 
    parsed from code parser
*/
typedef struct{
    char content[MAX_WORD_SIZE];
    int length;//, selected;
    CodeTokenType type;
} Token;

/*
    Struct: Line
    Each line is a doubly linked list of tokens
*/
typedef struct{
    LinkedList lineList;
    int length;
} Line;

/*
    Struct: Passage
    Each passage is a doubly linked list of lines
*/
typedef struct{
    LinkedList passList;
} Passage;

typedef struct {
    int r, c;
} PosRC;

static char* KeyWord[32] = {"auto","break","case","char","const","continue","default","do",\
                    "double","else","enum","extern","float","for","goto","if",\
                    "int","long","register","return","short","signed","sizeof","static",\
                    "struct","switch","typedef","union","unsigned","void","volatile","while"};
/*
    Function: initPassage
    Initialize doubly linked list for a passage
*/
void initPassage(Passage *p);

/*
    Function: getline(Passage *passage, char *dst, int row)
    store all the content in dst in line by row
    returns the total length of this line
    **It will OVERWRITE data in dst.
*/ 
int getLine(Passage *passage, char *dst, int row);

/*
    Function: deleteLine(Passage *passage, int row)
    delete line by row and free the memory
*/ 
void deleteLine(Passage *passage, int row);

/*
    Function: parseLine(Passage *passage, int row)
    parse line by row
    return the curpos of the last character
*/ 
int parseLine(Passage *passage, int row);

/*
    Function: getPos(Passage *p, int row, int col, int *offset)
    Find the char by row and col index
    returns the pointer to that node, and gives the offset to variable <offset>
*/ 
Listptr getPos(Passage *passage, int row, int col, int *offset);

/*
    Function: addString(Passage *p, char *str, int row, int col)
    Insert string into the passage at given position, 
    which is represented by row and col index.
    If give row is not in the passage, then create a new row in the list.
*/ 
PosRC addString(Passage *passage, char *str, int row, int col);

/*
    Function: deleteString(Passage *p, int rows, int cols, int rowt, int colt)
    Delete string in the passage at given range, (closed interval)
    which is represented by row and col index.
    If want to delete the empty line(contaning only \n) at i colomn, need to call:
        deleteString(passage, i-1, (i-1).length, i, 1)
*/ 
void deleteString(Passage *passage, int rows, int cols, int rowt, int colt);

/*
    Function: getString(Passage *passage, int rows, int cols, int rowt, int colt)
    Return the pointer to a string in range <(rows,cols),(rowt,colt)>
    **Caller have the duty to free the allocated space
*/
char *getString(Passage *passage, int rows, int cols, int rowt, int colt);

void inputString(char *str);

/*
    Function: searchForwardByChar(Passage *passage, int row, int col, char ch)
    Search for <ch> start from <(row, col)>
    Return the position by <posRC>
    If it is not found, return <{-1, -1}>
*/ 
PosRC searchForwardByChar(Passage *passage, int row, int col, char ch);
/*
    Function: searchBackwardByChar(Passage *passage, int row, int col, char ch)
    Search for <ch> before <(row, col)>
    Return the position by <posRC>
    If it is not found, return <{-1, -1}>
*/ 
PosRC searchBackwardByChar(Passage *passage, int row, int col, char ch);

// debug use
void printPassage(Passage *p);
#endif
