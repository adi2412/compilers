/* Compilers Project Group 2
// Aditya Raisinghani 2011A7PS042P
// Utkarsh Verma 2011A7PS137P
// BITS Pilani, Pilani Campus
// Second semester 2014
*/

// Contains function prototype declarations of functions in lexer.c

#ifndef LEXER
#define LEXER
#define BUFFERSIZE 1024

typedef int buffersize;

typedef struct _buffer buffStruct;
typedef buffStruct *buffer;

// Buffer structure
struct _buffer
{
	char buff[BUFFERSIZE];
	int curPointer;
	int fwdPointer;
	int lineNumber;
	int charNumber;
	buffer nextBuffer;
};

FILE *getStream(FILE *fp, buffer B, buffersize k);
tokenInfo getNextToken(FILE *fp, buffer B);
tokenInfo	lexer(FILE *fp, FILE *dest);
char* getTokenName(token name);

#endif