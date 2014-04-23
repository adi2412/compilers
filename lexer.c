/* Compilers Project Group 2
// Aditya Raisinghani 2011A7PS042P
// Utkarsh Verma 2011A7PS137P
// BITS Pilani, Pilani Campus
// Second semester 2014
*/

//LEGEND:
//UPDATE: need more information to decide
//!MODIFY!: possibly wrong, need to modify accordingly
//CONSIDER: need to think this through

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tokens.h"
#include "lexer.h"
#include "parserDef.h"
#include "first_follow_gen.h"
#include "parser.h"	// Might be changed to another header file for the tokens.


#define FUNSIZE 20 // Don't know the correct value currently UPDATE
#define IDSIZE 20 //UPDATE
#define NUMSIZE 20 //UPDATE

buffer curBuff; // Have to globally assign otherwise it doesn't seem to work.

/*
// Shows all the different errors.
// TODO: Figure out the correct implementation of error.
*/
void showError(char *errtext,int errline)
{
	printf("\n\x1b[31mError generated!\n\x1b[0m");
	printf("\n\x1b[31m %s At Line no: %d\n\x1b[0m", errtext,errline);	
	//exit(0);		//DONT need to exit. will continue after printing error.
}

FILE *getStream(FILE *fp, buffer B, buffersize k)
{
	// calls the fgets function.
	if(fgets(B->buff, k, fp) == NULL)
	{
		// No more lines found.
		B = NULL;
		return NULL;
	}
	//printf("%p, --> %p",B, B->nextBuffer);
	// set the character number to the value from the previous buffer
	else
	{
		B->nextBuffer->curPointer = 0;
		B->nextBuffer->fwdPointer = 0;
		B->charNumber = B->nextBuffer->charNumber;
		B->lineNumber = B->nextBuffer->lineNumber;
		curBuff = B;
		return fp;
	}
	// and we're done :/
}

tokenInfo getEndCommentAndNextToken(FILE *fp, buffer B)
{
	//currently, a comment body is being traversed. we need to look for comment end and then return next token
	//will start incrementing fwdPointer within while loop
	while(B->fwdPointer < BUFFERSIZE && (B->buff[B->fwdPointer]!= '\n' && B->buff[B->fwdPointer]!= EOF))
		{
			++B->fwdPointer;
		}
		// Any of the above cases means we have reached the end of the current buffer. Check if exceeded buffer length

	if(B->buff[B->fwdPointer] == '\n' || B->buff[B->fwdPointer] == EOF)
	{
		// The comment has ended. Get the next token and return it.
		++B->lineNumber;
		FILE* blah = getStream(fp, B->nextBuffer, BUFFERSIZE);
		// Set current buffer to next buffer
		if(blah == NULL || B->nextBuffer == NULL)
		{
			return NULL;
		}
		B = B->nextBuffer;
		// Keep checking for how long comments go and return next token.
		return getNextToken(fp, B);
	}
	if(B->fwdPointer >= BUFFERSIZE)
	{
		// The comment has overflowed to next buffer. Finish reading comment and return the next token.
		FILE* blah = getStream(fp, B->nextBuffer, BUFFERSIZE);
		// Set current buffer to next buffer
		if(blah == NULL || B->nextBuffer == NULL)
		{
			return NULL;
		}
		B = B->nextBuffer;
		// Keep checking for how long comments go and return next token.
		return getEndCommentAndNextToken(fp, B);
	}
	return NULL;
		//hope this works
		//is a return needed for default case? the 2 if blocks above should be sufficient..
		//check website for info if error to be returned after 2 buffers full of comment. UPDATE
}

char* joinStrings(char* alp, char* str)
{
	int i = 1;
	for(;i<=strlen(str);++i)
	{
		alp[i] = str[i-1];
	}
	alp[i] = '\0';
	return alp;
}

tokenInfo getFunctionToken(FILE *fp, buffer B, int i)
{
	// Find the function token. 
	if(i==0)
	{
		// Name length exceeded
		char *err= "Function name length exceeded.";
		showError(err,B->lineNumber);
		return NULL;
	}
	tokenInfo funToken = malloc(sizeof(tokenStruct));
	char* alphabet = malloc(i*sizeof(char));
	// Check if the buffer has enough space. Else change the buffer.
	if(B->fwdPointer >= BUFFERSIZE)
	{
		// Change buffers.
		getStream(fp, B->nextBuffer, BUFFERSIZE);
		// Set current buffer to next buffer
		if(B->nextBuffer == NULL)
		{
			return NULL;
		}
		B = B->nextBuffer;
		// Return next token from the next buffer.
		return getFunctionToken(fp, B, i);
	}
	alphabet[0] = B->buff[B->fwdPointer];
	if((alphabet[0] < 'a' || alphabet[0] > 'z') && (alphabet[0] < 'A' || alphabet[0] > 'Z') && (alphabet[0] < '0' || alphabet[0] > '9'))
	{
		// Function name ended on previous token.
		free(funToken);
		//free(alphabet);
		return NULL;
	}
	else
	{
		// It is an alphabet. Keep checking further.
		++B->fwdPointer;
		tokenInfo nextToken = getFunctionToken(fp, B, --i);
		if(nextToken == NULL)
		{
			funToken->token_value = alphabet;
			free(nextToken);
			//free(alphabet);
			return funToken;
		}
		else
		{
			funToken->token_value = joinStrings(alphabet,nextToken->token_value);
			free(nextToken);
			//free(alphabet);
			return funToken;
		}
	}
}

tokenInfo getIDToken(FILE *fp, buffer B, int i)
{
	// Find the identifier token. Return even if it is a keyword as an identifier
	if(i == 0)
	{
		char *err = "Token size passed to getIDToken() is zero.";
		showError(err, B->lineNumber);
		return NULL;
	}
	tokenInfo idToken = malloc(sizeof(tokenStruct));
	char* alphabet = (char*)malloc(i*sizeof(char));
	// Check if the buffer has enough space. Else change the buffer.
	if(B->fwdPointer >= BUFFERSIZE)
	{
		// Change buffers.
		getStream(fp, B->nextBuffer, BUFFERSIZE);
		// Set current buffer to next buffer
		if(B->nextBuffer == NULL)
		{
			return NULL;
		}
		B = B->nextBuffer;
		// Return next token from the next buffer.
		return getIDToken(fp, B, i);
	}
	alphabet[0] = B->buff[B->fwdPointer];
	if((alphabet[0] < 'a' || alphabet[0] > 'z') && (alphabet[0] < 'A' || alphabet[0] > 'Z') && (alphabet[0] < '0' || alphabet[0] > '9'))
	{
		// Function name ended on previous token.
		free(idToken);
		//free(alphabet);
		return NULL;
	}
	else
	{
		// It is an alphabet. Keep checking further.
		++B->fwdPointer;
		if(alphabet[0] >= '0' && alphabet[0] <= '9')
		{
			// Last letter of the alphabet. Return it right now.
			idToken->token_value = alphabet;
			//free(alphabet);
			return idToken;
		}
		tokenInfo nextToken = getIDToken(fp, B, --i);
		if(nextToken == NULL)
		{
			idToken->token_value = alphabet;
			free(nextToken);
			//free(alphabet);
			return idToken;
		}
		else
		{
			idToken->token_value = joinStrings(alphabet,nextToken->token_value);
			free(nextToken);
			//free(alphabet);
			return idToken;
		}
	}
}

tokenInfo getStringToken(FILE *fp, buffer B, int i)
{
	// Find the string text.
	if(i == 0)
	{
		char *err = "Token size passed to getStringToken() is zero.";
		showError(err, B->lineNumber);
		return NULL;
	}
	tokenInfo stringToken = malloc(sizeof(tokenStruct));
	char* alphabet = (char*)malloc(i*sizeof(char));
	// Check if the buffer has enough space. Else change the buffer.
	if(B->fwdPointer >= BUFFERSIZE)
	{
		// Change buffers.
		getStream(fp, B->nextBuffer, BUFFERSIZE);
		// Set current buffer to next buffer
		if(B->nextBuffer == NULL)
		{
			return NULL;
		}
		B = B->nextBuffer;
		// Return next token from the next buffer.
		return getStringToken(fp, B, i);
	}
	alphabet[0] = B->buff[B->fwdPointer];
	if((alphabet[0] < 'a' || alphabet[0] > 'z') && alphabet[0] != ' ')
	{
		// Not an alphabet. End here.
		free(stringToken);
		//free(alphabet);
		return NULL;
	}
	else
	{
		// It is an alphabet(or a space). Keep going on.
		++B->fwdPointer;
		tokenInfo nextToken = getStringToken(fp, B, --i);
		if(nextToken == NULL)
		{
			stringToken->token_value = alphabet;
			free(nextToken);
			//free(alphabet);
			return stringToken;
		}
		else
		{
			stringToken->token_value = joinStrings(alphabet,nextToken->token_value);
			//printf("%s",stringToken->token_value);
			//printf("%c",alphabet[0]);
			free(nextToken);
			//free(alphabet);
			return stringToken;
		}
	}
}

tokenInfo getNumberToken(FILE *fp, buffer B)
{	
	//integers, and real number upto 2 decimal places supported
	tokenInfo numToken = malloc(sizeof(tokenStruct));
	char* alphabet = (char*)malloc(NUMSIZE*sizeof(char));
	int i=0;
	// Check if the buffer has enough space. Else change the buffer.
	if(B->fwdPointer >= BUFFERSIZE)
	{
		// Change buffers.
		getStream(fp, B->nextBuffer, BUFFERSIZE);
		// Set current buffer to next buffer
		if(B->nextBuffer == NULL)
		{
			return NULL;
		}
		B = B->nextBuffer;
		// Return next token from the next buffer.
		return getNumberToken(fp, B);
	}
	// alphabet[i] = B->buff[B->fwdPointer];


	while(((B->buff[B->fwdPointer] >= '0') && (B->buff[B->fwdPointer] <= '9')) && i< NUMSIZE)  //UPDATE:how to handle the number size limit?
	{
		alphabet[i++] = B->buff[B->fwdPointer++];	
		//++B->fwdPointer; //included inline above
	}

	//keep moving pointer ahead of numbers, till we reach . or some another character (error)
	if(B->buff[B->fwdPointer] == '.')
	{	
		if(B->buff[(B->fwdPointer)+1] >='0' && B->buff[(B->fwdPointer)+1] <='9') //+1 to ptr to peek ahead and see if nos ahead, append the . if true
		{
			alphabet[i++] = B->buff[B->fwdPointer++];
		}
				
		int maxDecimalPlace=2; int currentDecimalPlace=0;
		while(((B->buff[B->fwdPointer] >= '0') && (B->buff[B->fwdPointer] <= '9')) && i< NUMSIZE && currentDecimalPlace < maxDecimalPlace)  //UPDATE:how to handle the number size limit?
		{
			alphabet[i++] = B->buff[B->fwdPointer++];	
			++currentDecimalPlace;
		}
		if(currentDecimalPlace==2) //  B->buff[B->fwdPointer] == '.')
		{	//it is obv upto maxDecimalPlace only by code design
			numToken->token_name = RNUM;
			numToken->token_value = alphabet; //CONSIDER:this will need atof conversion later.
			//free(alphabet);
			return numToken;
		}
		//CONSIDER:clean this commented mess up.
		// else if(currentDecimalPlace ==0) //non numeric char after '.'
		// {	
			
		// 	//alphabet contains the number before period '.', fwdPointer is at '.' currently
		// 	numToken->token_value = alphabet; //this will need atoi conversion later.
		// 	numToken->token_name = "NUM";
		// 	free(alphabet);
		// 	return numToken;
		// }
		else //currentDecimalPlace bw [0,2)
		{	//eg 24.5 is invalid, need 24.50 
			char *err = "Invalid real num. Need exactly 2 decimal places.";
			showError(err,B->lineNumber);
			free(numToken);
			//free(alphabet);
			return NULL;
		}
	}
	else
	{	//non numeric and non-period character found, return token
		numToken->token_value = alphabet; //this will need atoi conversion later.
		numToken->token_name = NUM;
		//free(alphabet);
		return numToken;	//CONSIDER: the fwdPointer has increasd. handle that?
	}
	return NULL;
}

tokenInfo createKeywordToken(token token_name)
{
	tokenInfo token = malloc(sizeof(tokenStruct));
	token->token_name = token_name;
	return token;
}

tokenInfo checkForKeyword(char* name)
{
	// Check if it is a keyword.
	// Add function as a keyword.
	if(strcmp(name,"main") && strcmp(name,"string") && strcmp(name,"end") && strcmp(name,"int") && strcmp(name,"real") && strcmp(name,"matrix") && strcmp(name,"if") && strcmp(name,"else") && strcmp(name,"endif") && strcmp(name,"read") && strcmp(name,"print") && strcmp(name,"function"))
	{
		// It is not a keyword. Return NULL.
		return NULL;
	}
	else
	{
		// It is a keyword. Check which one it is.
		if(!strcmp(name,"main"))
		{
			// Main keyword.
			return createKeywordToken(MAIN);
		}
		else if(!strcmp(name,"string"))
		{
			return createKeywordToken(STRING);
		}
		else if(!strcmp(name,"end"))
		{
			return createKeywordToken(END);
		}
		else if(!strcmp(name,"int"))
		{
			return createKeywordToken(INT);
		}
		else if(!strcmp(name,"real"))
		{
			return createKeywordToken(REAL);
		}
		else if(!strcmp(name,"matrix"))
		{
			return createKeywordToken(MATRIX);
		}
		else if(!strcmp(name,"if"))
		{
			return createKeywordToken(IF);
		}
		else if(!strcmp(name,"else"))
		{
			return createKeywordToken(ELSE);
		}
		else if(!strcmp(name,"endif"))
		{
			return createKeywordToken(ENDIF);
		}
		else if(!strcmp(name,"read"))
		{
			return createKeywordToken(READ);
		}
		else if(!strcmp(name,"print"))
		{
			return createKeywordToken(PRINT);
		}
		else if(!strcmp(name,"function"))
		{
			return createKeywordToken(FUNCTION);
		}
	}
	return NULL;
}

tokenInfo makeSingleToken(buffer B)
{
	// Creates a token for a single character and returns it. Reduces a lot of repititive work.
	tokenInfo token = malloc(sizeof(tokenStruct));
	token->charNumber = B->charNumber;
	token->lineNumber = B->lineNumber;
	//Increment values of curPointer and fwdPointer;
	++B->curPointer;
	++B->fwdPointer;
	++B->charNumber;
	return token;
}

tokenInfo getNextToken(FILE *fp, buffer B)
{
	// Read the input from the buffer.
	// Check if the buffer has enough space. Else change the buffer.
	if(B->fwdPointer >= BUFFERSIZE)
	{
		// Change buffers.
		getStream(fp, B->nextBuffer, BUFFERSIZE);
		// Set current buffer to next buffer
		if(B->nextBuffer == NULL)
		{
			return NULL;
		}
		B = B->nextBuffer;
		// Return next token from the next buffer.
		return getNextToken(fp, B);
	}
	char a = B->buff[B->fwdPointer];
	// Check first for the one character tokens.
	if(a == '(')
	{
		tokenInfo token = makeSingleToken(B);
		token->token_name = OP;
		return token;
	}
	else if(a == ')')
	{
		tokenInfo token = makeSingleToken(B);
		token->token_name = CL;
		return token;
	}
	else if(a == '[')
	{
		tokenInfo token = makeSingleToken(B);
		token->token_name = SQO;
		return token;
	}
	else if(a == ']')
	{
		tokenInfo token = makeSingleToken(B);
		token->token_name = SQC;
		return token;
	}
	else if(a == ';')
	{
		tokenInfo token = makeSingleToken(B);
		token->token_name = SEMICOLON;
		return token;
	}
	else if(a == ',')
	{
		tokenInfo token = makeSingleToken(B);
		token->token_name = COMMA;
		return token;
	}
	else if(a == '+')
	{
		tokenInfo token = makeSingleToken(B);
		token->token_name = PLUS;
		return token;
	}
	else if(a == '-')
	{
		tokenInfo token = makeSingleToken(B);
		token->token_name = MINUS;
		return token;
	}
	else if(a == '*')
	{
		tokenInfo token = makeSingleToken(B);
		token->token_name = MUL;
		return token;
	}
	else if(a == '/')
	{
		tokenInfo token = makeSingleToken(B);
		token->token_name = DIV;
		return token;
	}
	else if(a == '@')
	{
		tokenInfo token = makeSingleToken(B);
		token->token_name = SIZE;
		return token;
	}
	// Check for comments token and safely ignore
	else if(a == '#')
	{
		++B->fwdPointer;
		while(B->fwdPointer < BUFFERSIZE && (B->buff[B->fwdPointer]!= '\n' && B->buff[B->fwdPointer]!= EOF))
		{
			++B->fwdPointer;
		}
		// Any of the above cases means we have reached the end of the current buffer. Check if exceeded buffer length

		if(B->buff[B->fwdPointer] == '\n' || B->buff[B->fwdPointer] == EOF)
		{
			// The comment has ended. Get the next token and return it.
			++B->lineNumber;
			getStream(fp, B->nextBuffer, BUFFERSIZE);
			// Set current buffer to next buffer
			if(B->nextBuffer == NULL)
			{
				return NULL;
			}
			B = B->nextBuffer;
			// Keep checking for how long comments go and return next token.
			return getNextToken(fp, B);
		}

		if(B->fwdPointer >= BUFFERSIZE)
		{
			// The comment has overflowed to next buffer. Finish reading comment and return the next token.
			getStream(fp, B->nextBuffer, BUFFERSIZE);
			// Set current buffer to next buffer
			if(B->nextBuffer == NULL)
			{
				return NULL;
			}
			B = B->nextBuffer;
			// Keep checking for how long comments go and return next token.
			return getEndCommentAndNextToken(fp, B);
		}
	}
	else if(a == '>' || a == '<')
	{
		// Check next token.
		++B->fwdPointer;
		tokenInfo token = malloc(sizeof(tokenStruct));
		token->charNumber = B->charNumber;
		token->lineNumber = B->lineNumber;
		int curPointer = B->curPointer; // Store the current value temporarily.
		tokenInfo nextToken = getNextToken(fp, B);
		if(nextToken->token_name == ASSIGNOP)
		{
			// Assign the correct value to this token.
			if(a == '>')
			{
				token->token_name = GE;
				B->curPointer = ++B->fwdPointer;
				B->charNumber = B->fwdPointer;
				free(nextToken);
				return token;
			}
			else
			{
				token->token_name = LE;
				B->curPointer = ++B->fwdPointer;
				B->charNumber = B->fwdPointer;
				free(nextToken);
				return token;
			}
		}
		else
		{
			// Something else. Return LT or GT.
			// TODO: Is an error possible in this case?
			if(a == '>')
			{
				token->token_name = GT;
				// Lost the next token. Need to generate that one again.
				B->curPointer = curPointer + 1;
				B->fwdPointer = B->curPointer;
				B->charNumber = B->fwdPointer;
				free(nextToken);
				return token;
			}
			else if(a == '<')
			{
				token->token_name = LT;
				// Lost the next token. Need to generate that one again.
				B->curPointer = curPointer + 1;
				B->fwdPointer = B->curPointer;
				B->charNumber = B->fwdPointer;
				free(nextToken);
				return token;
			}
		}
	}
	else if(a == '=')
	{
		++B->fwdPointer;
		tokenInfo token = malloc(sizeof(tokenStruct));
		token->charNumber = B->charNumber;
		token->lineNumber = B->lineNumber;
		int curPointer = B->curPointer; // Temporary store.
		tokenInfo nextToken = getNextToken(fp, B);
		if(nextToken->token_name == ASSIGNOP)
		{
			// EQ token.
			token->token_name = EQ;
			B->charNumber = ++B->fwdPointer;
			free(nextToken);
			return token;
		}
		else if(nextToken->token_name == DIV)
		{
			// This should become a NE token
			if(B->buff[B->fwdPointer] == '=')
			{
				// It is the NE token.
				token->token_name = NE;
				++B->fwdPointer;
				++B->curPointer;
				++B->charNumber;
				free(nextToken);
				return token;
			}
			else
			{
				// Not NE token. An error
				// TODO: Decide error functions
				char *err = "Bad token. Possible Token: NE .";
				showError(err, B->lineNumber);
				++B->fwdPointer;
				++B->curPointer;
				++B->charNumber;
				free(nextToken);
				free(token);
				return NULL;
			}
		}
		else
		{
			// Something else. This is the ASSIGNOP token
			token->token_name = ASSIGNOP;
			// Lost the next token. Need to generate again.
			B->curPointer = ++curPointer;
			B->fwdPointer = B->curPointer;
			B->charNumber = B->fwdPointer;
			free(nextToken);
			return token;
		}
	}
	else if(a == '.')
	{
		// One of the logical operators, possibly
		++B->fwdPointer;
		tokenInfo token = malloc(sizeof(tokenStruct));
		token->charNumber = B->charNumber;
		token->lineNumber = B->lineNumber;
		int curPointer = B->curPointer;
		tokenInfo nextToken = getNextToken(fp, B);
		if(nextToken->token_name == ID)
		{
			// Check which of the logical operators
			if(!strcmp(nextToken->token_value,"and"))
			{
				// Maybe the and operator.
				if(B->buff[B->fwdPointer] == '.')
				{
					// AND token.
					token->token_name = AND;
					// Increment current and fwd pointer.
					++B->fwdPointer;
					++B->curPointer;
					++B->charNumber;
					free(nextToken);
					return token;
				}
				else
				{
					// Something else. A Lexical error;
					char *err = "Lexical error. Possible Token: AND .";
					showError(err, B->lineNumber);
					++B->fwdPointer;
					++B->curPointer;
					++B->charNumber;
					free(nextToken);
					free(token);
					return NULL;
				}
			}
			else if(!strcmp(nextToken->token_value,"or"))
			{
				// Maybe the or operator
				if(B->buff[B->fwdPointer] == '.')
				{
					// OR token.
					token->token_name = OR;
					++B->fwdPointer;
					++B->curPointer;
					++B->charNumber;
					free(nextToken);
					return token;
				}
				else{
					// Lexical error.
					char *err = "Lexical error. Possible token: OR .";
					showError(err, B->lineNumber);
					++B->fwdPointer;
					++B->curPointer;
					++B->charNumber;
					free(nextToken);
					free(token);
					return NULL;
				}
			}
			else if(!strcmp(nextToken->token_value,"not"))
			{
				// Maybe the not operator
				if(B->buff[B->fwdPointer] == '.')
				{
					// NOT token.
					token->token_name = NOT;
					++B->fwdPointer;
					++B->curPointer;
					++B->charNumber;
					free(nextToken);
					return token;
				}
				else{
					// Lexical error.
					char *err = "Lexical error. Possible Token: NOT .";
					showError(err, B->lineNumber);
					++B->fwdPointer;
					++B->curPointer;
					++B->charNumber;
					free(nextToken);
					free(token);
					return NULL;
				}
			}
			else
			{
				// Some other identifier. A Lexical error;
				char *err = "Lexical error after .";
				showError(err, B->lineNumber);
				B->curPointer = ++curPointer;
				B->fwdPointer = B->curPointer;
				B->charNumber = B->fwdPointer;
				free(nextToken);
				free(token);
				return NULL;
			}
		}
		else
		{
			// A Lexical error;
			char *err = "Lexical Error after .";
			showError(err,B->lineNumber);
			B->curPointer = ++curPointer;
			B->fwdPointer = B->curPointer;
			B->charNumber = B->fwdPointer;
			free(nextToken);
			free(token);
			return NULL;
		}
	}
	else if(a == '_')
	{
		// A function identifier
		++B->fwdPointer;
		tokenInfo token = malloc(sizeof(tokenStruct));
		token->charNumber = B->charNumber;
		token->lineNumber = B->lineNumber;
		token->token_name = FUNID;
		char* funvalue = (char*)malloc((1+FUNSIZE)*sizeof(char));
		funvalue[0] = '_';
		if((B->buff[B->fwdPointer] >= 'a' && B->buff[B->fwdPointer] <= 'z') || (B->buff[B->fwdPointer] >= 'A' && B->buff[B->fwdPointer] <= 'Z'))
		{
			// Check for the function token.
			tokenInfo funToken = getFunctionToken(fp, B, FUNSIZE);
			strcpy(funvalue,funToken->token_value);
			// No need to store the underscore of the function name.
			tokenInfo keywordToken = checkForKeyword(funToken->token_value);
			if(keywordToken != NULL)
			{
				// It is a keyword token. Return as keyword.
				free(token);
				token = keywordToken;
				token->charNumber = B->charNumber;
				token->lineNumber = B->lineNumber;
				B->curPointer = B->fwdPointer;
				B->charNumber = B->fwdPointer;
				free(funvalue);
				//free(keywordToken);
				free(funToken);
				return token;
			}
			else
			{
				// It is an identifier. Return the identifier itself.
				token->token_value = funvalue;
				B->curPointer = B->fwdPointer;
				B->charNumber = B->fwdPointer;
				free(funToken);
				return token;
			}
		}
		else
		{
			// Incorrect. Show error.
			char *err = "Lexical Error. Function names may start only with alphabets.";
			showError(err, B->lineNumber);
			B->curPointer = B->fwdPointer;
			B->charNumber = B->fwdPointer;
			free(funvalue);
			free(token);
			return NULL;
		}
	}
	else if((a >= 'a' && a<= 'z') || (a >= 'A' && a<= 'Z'))
	{
		// An alphabet identifier
		++B->fwdPointer;
		tokenInfo token = malloc(sizeof(tokenStruct));
		token->charNumber = B->charNumber;
		token->lineNumber = B->lineNumber;
		token->token_name = ID;
		char* idvalue = (char*)malloc((1+IDSIZE)*sizeof(char));
		tokenInfo idToken = getIDToken(fp, B, IDSIZE);
		// Check if it is a token
		idvalue[0] = a;
		if(idToken == NULL)
		{
			// Check for one alphabet identifier only.
			token->token_value = idvalue;
			B->curPointer = B->fwdPointer;
			B->charNumber = B->fwdPointer;
			free(idToken);
			return token;
		}
		idvalue = joinStrings(idvalue,idToken->token_value);	
		idToken->token_value = idvalue;
		tokenInfo keywordToken = checkForKeyword(idToken->token_value);
		if(keywordToken != NULL)
		{
			// It is a keyword token. Return as keyword.
			free(token);
			token = keywordToken;
			token->charNumber = B->charNumber;
			token->lineNumber = B->lineNumber;
			B->curPointer = B->fwdPointer;
			B->charNumber = B->fwdPointer;
			//free(keywordToken);
			free(idToken);
			return token;
		}
		else
		{
			// It is an identifier. Return the identifier itself.
			token->token_value = idvalue;
			//printf("%s",token->token_value);
			B->curPointer = B->fwdPointer;
			B->charNumber = B->fwdPointer;
			free(keywordToken);
			free(idToken);
			return token;
		}	
	}
	else if(a >= '0' && a<= '9')
	{
		// A number identifier
		++B->fwdPointer;
		tokenInfo token = malloc(sizeof(tokenStruct));
		token->charNumber = B->charNumber;
		token->lineNumber = B->lineNumber;
		char* numvalue = (char*)malloc((1+NUMSIZE)*sizeof(char));
		tokenInfo numToken = getNumberToken(fp, B);
		token->token_name = numToken->token_name; //whether RealNum or IntNum.
		numvalue[0] = a;
		token->token_value = joinStrings(numvalue,numToken->token_value);
		B->curPointer = B->fwdPointer;
		B->charNumber = B->fwdPointer;
		free(numToken);
		return token;
	}
	else if(a == '\"')
	{
		// String type.
		++B->fwdPointer;
		tokenInfo token = malloc(sizeof(tokenStruct));
		token->charNumber = B->charNumber;
		token->lineNumber = B->lineNumber;
		token->token_name = STR;
		char* stringValue = (char*)malloc((IDSIZE)*sizeof(char));
		tokenInfo stringToken = getStringToken(fp, B, IDSIZE);
		// CONSIDER: check if end of string has a '"'
		if(B->buff[B->fwdPointer] == '"')
		{
			// It is a string ending with the '"'
			strcpy(stringValue,stringToken->token_value);
			token->token_value = stringValue;
			++B->fwdPointer;
			//printf("%s +++ %s",token->token_value,stringToken->token_value);
			B->curPointer = B->fwdPointer;
			B->charNumber = B->fwdPointer;
			free(stringToken);
			return token;
		}
		else
		{
			// It is not a token. Lexical error
			char *err = "Lexical error after \\";
			showError(err,B->lineNumber);
			++B->fwdPointer;
			B->curPointer = B->fwdPointer;
			B->charNumber = B->fwdPointer;
			free(stringToken);
			free(token);
			free(stringValue);
			return NULL;
		}
	}
	else
	{
		// All other cases.
		if(a == '\n')
		{
			// New line. Change values of line number and char number
			++B->lineNumber;
			B->charNumber = 0;
			// Change the buffer.
			FILE* check = getStream(fp, B->nextBuffer, BUFFERSIZE);
			// Set current buffer to next buffer
			if(check == NULL)
			{
				return NULL;
			}
			B = B->nextBuffer;
			return getNextToken(fp, B);
		}
		else if(a == ' ')
		{
			++B->fwdPointer;
			B->curPointer = B->fwdPointer;
			++B->charNumber;
			return getNextToken(fp, B);
		}
		else if(a == EOF)
		{
			// End of the input file. Stop here.
			return NULL;
		}
		else if(a == '\t')
		{
			++B->fwdPointer;
			B->curPointer = B->fwdPointer;
			++B->charNumber;
			return getNextToken(fp, B);
		}
		else
		{
			// Don't know what type of cases come here. CONSIDER
			return NULL;
		}
	}
	return NULL;
}

char* getTokenName(token name)
{
	switch(name)
	{
		case MAIN: return "MAIN";
		case FUNID: return "FUNID";
		case SEMICOLON: return "SEMICOLON";
		case STRING: return "STRING";
		case STR: return "STR";
		case INT: return "INT";
		case ASSIGNOP: return "ASSIGNOP";
		case COMMENT: return "COMMENT";
		case ID: return "ID";
		case NUM: return "NUM";
		case RNUM: return "RNUM";
		case END: return "END";
		case REAL: return "REAL";
		case MATRIX: return "MATRIX";
		case SQO: return "SQO";
		case SQC: return "SQC";
		case OP: return "OP";
		case CL: return "CL";
		case PLUS: return "PLUS";
		case MINUS: return "MINUS";
		case MUL: return "MUL";
		case DIV: return "DIV";
		case COMMA: return "COMMA";
		case SIZE: return "SIZE";
		case IF: return "IF";
		case ELSE: return "ELSE";
		case ENDIF: return "ENDIF";
		case READ: return "READ";
		case PRINT: return "PRINT";
		case AND: return "AND";
		case OR: return "OR";
		case NOT: return "NOT";
		case LT: return "LT";
		case LE: return "LE";
		case GT: return "GT";
		case GE: return "GE";
		case EQ: return "EQ";
		case NE: return "NE";
		case FUNCTION: return "FUNCTION";
		case DOL: return "DOLLAR";
		case NIL: return "EPSILON";
		default: return "Kuch aur";
	}
}

char* getLexemeName(tokenInfo tokens)
{
	if(tokens->token_name == STR || tokens->token_name == FUNID || tokens->token_name == ID || tokens->token_name == NUM || tokens->token_name == RNUM)
	{
		return tokens->token_value;
	}
	else
		return getTokenName(tokens->token_name);
}

/*
//	The main function of the lexer which is called by the parser.
//	params-
//		input: pointer to file.
//		output: header to the first token which forms a linked list.
*/
tokenInfo lexer(FILE *source, FILE* dest)
{
	// FILE *source;
	// source = fopen("test.txt","r");
	tokenInfo tokens;
	buffer firstBuff = malloc(sizeof(buffStruct));	
	firstBuff->curPointer = 0;
	firstBuff->fwdPointer = 0;
	firstBuff->charNumber = 1;
	firstBuff->lineNumber = 1;
	buffer secondBuff = malloc(sizeof(buffStruct));
	secondBuff->curPointer = 0;
	secondBuff->fwdPointer = 0;
	secondBuff->charNumber = 1;
	secondBuff->lineNumber = 1;
	firstBuff->nextBuffer = secondBuff;
	secondBuff->nextBuffer = firstBuff;
	int lineNumber = 1;
	// Initialise the first buffer stream
	getStream(source, firstBuff, BUFFERSIZE);
	curBuff = firstBuff; // Current buffer being used.
	tokens = getNextToken(source,curBuff);
	tokenInfo head;
	if(tokens == NULL)
	{
		return NULL;
	}
	else{
		head = tokens;
		printf("%s ",getTokenName(tokens->token_name));
		if(tokens->token_name == ID || tokens->token_name == STR || tokens->token_name == FUNID || tokens->token_name == NUM || tokens->token_name == RNUM)
		{
			printf("(\x1b[37m%s\x1b[0m) ",tokens->token_value);
		}
		tokens->nextToken = NULL;
	}
	if(firstBuff->nextBuffer == NULL)
	{
		return 0;
	}
	while(curBuff->buff[curBuff->curPointer] != EOF)
	{
		tokenInfo nextTokens = getNextToken(source, curBuff);
		if(nextTokens == NULL)
		{
			// End of file reached. Stop here.
			break;
		}
		else
		{
			tokens->nextToken = nextTokens;
			if(nextTokens->lineNumber != lineNumber)
			{
				printf("\n");
				//curBuff->lineNumber = tokens->lineNumber;
				++lineNumber;
			}
			printf("%s ",getTokenName(nextTokens->token_name));
			if(nextTokens->token_name == ID || nextTokens->token_name == STR || nextTokens->token_name == FUNID)
			{
				printf("(\x1b[37m%s\x1b[0m) ",nextTokens->token_value);
			}
			tokens = tokens->nextToken;
			tokens->nextToken = NULL;
		}
	}
	tokens = head;
	printf("\n");
	fclose(source);
	return head;
}
