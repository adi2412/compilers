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
//#include 'lexer.h'	// Might be changed to another header file for the tokens.
#include "tokens.h"

#define BUFFERSIZE 1024
#define FUNSIZE 20 // Don't know the correct value currently UPDATE
#define IDSIZE 20 //UPDATE
#define NUMSIZE 20 //UPDATE

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

tokenInfo getNextToken(FILE *fp, buffer B);
/*
// Shows all the different errors.
// TODO: Figure out the correct implementation of error.
*/
void showError()
{
	printf("Error generated!");
}

FILE *getStream(FILE *fp, buffer B, buffersize k)
{
	// calls the fgets function.
	if(fgets(B->buff, k, fp) == NULL)
	{
		// No more lines found.
		free(B);
		free(B->nextBuffer);
		return NULL;
	}
	// set the character number to the value from the previous buffer
	B->nextBuffer->curPointer = 0;
	B->nextBuffer->fwdPointer = 0;
	B->charNumber = B->nextBuffer->charNumber;
	B->lineNumber = B->nextBuffer->lineNumber;
	return fp;
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
	printf("%s",alp);
	return alp;
}

tokenInfo getFunctionToken(FILE *fp, buffer B, int i)
{
	// Find the function token. This might be incorrect
	if(i==0)
	{
		// Name length exceeded
		showError();
		return NULL;
	}
	tokenInfo funToken = malloc(sizeof(tokenStruct));
	char* alphabet = malloc(sizeof(char));
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
		free(alphabet);
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
			free(alphabet);
			return funToken;
		}
		else
		{
			funToken->token_value = joinStrings(alphabet,nextToken->token_value);
			free(nextToken);
			free(alphabet);
			return funToken;
		}
	}
}

tokenInfo getIDToken(FILE *fp, buffer B, int i)
{
	// Find the identifier token. Return even if it is a keyword as an identifier
	if(i == 0)
	{
		showError();
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
		free(alphabet);
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
			free(alphabet);
			return idToken;
		}
		tokenInfo nextToken = getIDToken(fp, B, --i);
		if(nextToken == NULL)
		{
			idToken->token_value = alphabet;
			free(nextToken);
			free(alphabet);
			return idToken;
		}
		else
		{
			idToken->token_value = joinStrings(alphabet,nextToken->token_value);
			free(nextToken);
			free(alphabet);
			return idToken;
		}
	}
}

tokenInfo getStringToken(FILE *fp, buffer B, int i)
{
	// Find the string text.
	if(i == 0)
	{
		showError();
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
		free(alphabet);
		return NULL;
	}
	else
	{
		// It is an alphabet(or a space). Keep going on.
		++B->fwdPointer;
		tokenInfo nextToken = getFunctionToken(fp, B, --i);
		if(nextToken == NULL)
		{
			stringToken->token_value = alphabet;
			free(nextToken);
			free(alphabet);
			return stringToken;
		}
		else
		{
			stringToken->token_value = joinStrings(alphabet,nextToken->token_value);
			free(nextToken);
			free(alphabet);
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
	alphabet[i++] = B->buff[B->fwdPointer];


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
			free(alphabet);
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
			showError();
			free(numToken);
			free(alphabet);
			return NULL;
		}
	}
	else
	{	//non numeric and non-period character found, return token
		numToken->token_value = alphabet; //this will need atoi conversion later.
		numToken->token_name = NUM;
		free(alphabet);
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
		printf("not a keyword");
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
			printf("worked");
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
				B->curPointer = B->fwdPointer;
				B->charNumber = B->fwdPointer;
				free(nextToken);
				return token;
			}
			else
			{
				token->token_name = LE;
				B->curPointer = B->fwdPointer;
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
			B->charNumber = B->fwdPointer;
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
				showError();
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
					showError();
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
					showError();
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
					showError();
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
				showError();
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
			showError();
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
			// No need to store the underscore of the function name.
			tokenInfo keywordToken = checkForKeyword(funToken->token_value);
			if(keywordToken != NULL)
			{
				// It is a keyword token. Return as keyword.
				free(token);
				token = keywordToken;
				token->charNumber = B->charNumber;
				B->curPointer = B->fwdPointer;
				B->charNumber = B->fwdPointer;
				free(keywordToken);
				free(funToken);
				return token;
			}
			else
			{
				// It is an identifier. Return the identifier itself.
				token->token_value = funToken->token_value;
				B->curPointer = B->fwdPointer;
				B->charNumber = B->fwdPointer;
				free(funvalue);
				free(funToken);
				free(keywordToken);
				return token;
			}
		}
		else
		{
			// Incorrect. Show error.
			showError();
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
		tokenInfo keywordToken = checkForKeyword(idToken->token_value);
		if(keywordToken != NULL)
		{
			// It is a keyword token. Return as keyword.
			free(token);
			token = keywordToken;
			token->charNumber = B->charNumber;
			B->curPointer = B->fwdPointer;
			B->charNumber = B->fwdPointer;
			free(keywordToken);
			free(idToken);
			return token;
		}
		else
		{
			// It is an identifier. Return the identifier itself.
			token->token_value = joinStrings(idvalue,idToken->token_value);
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
		tokenInfo stringToken = getStringToken(fp, B, IDSIZE);
		// CONSIDER: check if end of string has a '"'
		if(B->buff[B->fwdPointer] == '\"')
		{
			// It is a string ending with the '"'
			token->token_value = stringToken->token_value;
			B->curPointer = B->fwdPointer;
			B->charNumber = B->fwdPointer;
			free(stringToken);
			return token;
		}
		else
		{
			// It is not a token. Lexical error
			showError();
			++B->fwdPointer;
			B->curPointer = B->fwdPointer;
			B->charNumber = B->fwdPointer;
			free(stringToken);
			free(token);
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
			getStream(fp, B->nextBuffer, BUFFERSIZE);
			// Set current buffer to next buffer
			if(B->nextBuffer == NULL)
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
		else
		{
			// Don't know what type of cases come here. CONSIDER
		}
	}
	return NULL;
}

/*
//	The main function of the lexer which is called by the parser.
//	params-
//		input: pointer to file.
//		output: header to the first token which forms a linked list.
*/
tokenInfo lexer(FILE *fp)
{
	FILE *source = fp;
	// Initialise tokenInfo
	tokenInfo tokens = malloc(sizeof(tokenStruct));
	tokens->nextToken = NULL;
	tokenInfo head = tokens;	//Points to head token
	buffer firstBuff = (buffer)malloc(sizeof(buffer));
	// Initialise the first buffer stream
	getStream(fp, firstBuff, BUFFERSIZE);
	firstBuff->curPointer = 0;
	firstBuff->fwdPointer = 0;
	firstBuff->charNumber = 1;
	firstBuff->lineNumber = 1;
	buffer curBuff = firstBuff; // Current buffer being used.
	buffer secondBuff = (buffer)malloc(sizeof(buffer));
	secondBuff->curPointer = 0;
	secondBuff->fwdPointer = 0;
	secondBuff->charNumber = 1;
	secondBuff->lineNumber = 1;
	firstBuff->nextBuffer = secondBuff;
	secondBuff->nextBuffer = firstBuff;
	while(curBuff->buff[firstBuff->curPointer])	//not sure if this works. need to compile and check
	{
		/* Send to state checker. */
		// Handle when buffer is not enough? Here or in getNextToken?
		// getNextToken should always return some token or the other.
		tokens = getNextToken(fp, curBuff);
		// Set character position of next token even before it is made.
		//tokens->charNumber = charNumber;
		if(tokens == NULL)
		{
			// The current buffer ran out while reading. Use other buffer.
			if(curBuff == firstBuff)
			{
				// The first buffer was current. Use the second.
				getStream(fp, secondBuff, BUFFERSIZE);
				// Do something about null being return by getStream.
				curBuff = secondBuff;
			}
			else
			{
				// The second buffer was current.
				getStream(fp, firstBuff, BUFFERSIZE);
				// Do something about null being returned by getStream.
				curBuff = firstBuff;
			}
		}
		else
		{
			// correct token returned!
			tokens->lineNumber = curBuff->lineNumber;
			// Initialize the next token in the linked list.
			tokens->nextToken = malloc(sizeof(tokenStruct));
			// Set value of charNumber to current position of pointer
			//charNumber = curBuff->curPointer;
			// Set tokens to point to next tokens.
			tokens = tokens->nextToken;
			tokens->nextToken = NULL;
		}
	} // Here's hopin this kind of works.
	return NULL;
}

char* getTokenName(token name)
{
	switch(name)
	{
		case MAIN: return "Main"; break;
		case FUNID: return "FUNID"; break;
		case SEMICOLON: return "SEMICOLON"; break;
		case STRING: return "STRING"; break;
		case STR: return "STR"; break;
		case INT: return "INT"; break;
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
		case NE: return "NE";
		case EQ: return "EQ";
		case FUNCTION: return "FUNCTION";
		default: return "Kuch aur"; break;
	}
}

int main()
{
	FILE *source;
	source = fopen("test.txt","r");
	tokenInfo tokens;
	buffer firstBuff = malloc(sizeof(buffStruct));	
	firstBuff->curPointer = 0;
	firstBuff->fwdPointer = 0;
	firstBuff->charNumber = 1;
	firstBuff->lineNumber = 1;
	buffer curBuff = firstBuff; // Current buffer being used.
	buffer secondBuff = malloc(sizeof(buffStruct));
	secondBuff->curPointer = 0;
	secondBuff->fwdPointer = 0;
	secondBuff->charNumber = 1;
	secondBuff->lineNumber = 1;
	firstBuff->nextBuffer = secondBuff;
	secondBuff->nextBuffer = firstBuff;
	// Initialise the first buffer stream
	getStream(source, firstBuff, BUFFERSIZE);
	if(firstBuff->nextBuffer == NULL)
	{
		return 0;
	}
	while(curBuff->buff[firstBuff->curPointer] != EOF)
	{
		tokens = getNextToken(source, curBuff);
		if(tokens == NULL)
		{
			// End of file reached. Stop here.
			break;
		}
		else
		{
			tokens->nextToken = malloc(sizeof(tokenStruct));
			printf("%s\n",getTokenName(tokens->token_name));
			if(tokens->token_name == ID)
				printf("%s\n",tokens->token_value);
			tokens = tokens->nextToken;
			tokens->nextToken = NULL;
		}
	}
	fclose(source);
	return 0;
}