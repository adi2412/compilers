/* Compilers Project Group 2
// Aditya Raisinghani 2011A7PS042P
// Utkarsh Verma 2011A7PS137P
// BITS Pilani, Pilani Campus
// Second semester 2014
*/
#include <stdlib.h>
#include 'lexer.h'	// Might be changed to another header file for the tokens.

#define BUFFERSIZE 1024

typedef int buffersize;
typedef char* buffer;

// Buffer structure
typedef struct _buffer
{
	char* buff;
	int curPointer;
	int fwdPointer;
} *buffer;

FILE *getStream(FILE *fp, buffer B, buffersize k)
{
	// calls the fgets function.
	fgets(B->buff, k, fp);
	// and we're done :/
}

tokenInfo getNextToken(buffer B)
{

}

/*
//	The main function of the lexer which is called by the parser.
//	params-
//		input: pointer to file.
//		output: header to the first token which forms a linked list.
*/
tokenInfo lexer(File *fp)
{
	FILE *source = fp;
	// Initialise tokenInfo
	tokenInfo tokens = (tokenInfo)malloc(sizeof(tokenInfo));
	tokenInfo head = tokens;	//Points to head token
	buffer firstBuff = (buffer)malloc(sizeof(buffer));
	// Initialise the first buffer stream
	getStream(fp, firstBuff, BUFFERSIZE);
	firstBuff->curPointer = 0;
	firstBuff->fwdPointer = 0;
	buffer curBuff = firstBuff; // Current buffer being used.
	buffer secondBuff = (buffer)malloc(sizeof(buffer));
	secondBuff->curPointer = 0;
	secondBuff->fwdPointer = 0;
	int lineNumber = 0;
	while(curBuff->buff[firstBuff->curPointer])	//not sure if this works. need to compile and check
	{
		/* Send to state checker. */
		// Handle when buffer is not enough? Here or in getNextToken?
		tokens = getNextToken(curBuff);
		if(tokens == null)
		{
			// The current buffer ran out while reading. Use other buffer.
			if(curBuff == firstBuff)
			{
				// The first buffer was current. Use the second.
				getstream(fp, secondBuff, BUFFERSIZE);
				curBuff = secondBuff;
			}
			else
			{
				// The second buffer was current.
				getstream(fp, firstBuff, BUFFERSIZE);
				curBuff = firstBuff;
			}
		}
	} // Here's hopin this kind of works.
}