/* Compilers Project Group 2
// Aditya Raisinghani 2011A7PS042P
// Utkarsh Verma 2011A7PS137P
// BITS Pilani, Pilani Campus
// Second semester 2014
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include 'lexer.h'	// Might be changed to another header file for the tokens.
#include 'tokens.h'

#define BUFFERSIZE 1024
#define FUNSIZE 20 // Don't know the correct value currently
#define IDSIZE 20

typedef int buffersize;

// Buffer structure
typedef struct _buffer
{
	char buff[BUFFERSIZE];
	int curPointer;
	int fwdPointer;
	int lineNumber;
	buffer nextBuffer;
} *buffer;


/*
// Shows all the different errors.
// TODO: Figure out the correct implementation of error.
*/
void showError()
{

}

FILE *getStream(FILE *fp, buffer B, buffersize k)
{
	// calls the fgets function.
	fgets(B->buff, k, fp);
	// and we're done :/
}

tokenInfo getEndCommentAndNextToken(buffer B)
{

}

char* joinStrings(char* alp, char* str)
{
	int i = 1;
	for(;i<=strlen(str);++i)
	{
		alp[i] = str[i-1];
	}
	return alp;
}

tokenInfo getFunctionToken(buffer B, int i)
{
	// Find the function token. This might be incorrect
	if(i==0)
	{
		// Name length exceeded
		showError();
		return NULL;
	}
	tokenInfo funToken = malloc(sizeof(tokenInfo));
	char* alphabet = malloc(i*sizeof(char));
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
		tokenInfo nextToken = getFunctionToken(B, --i);
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

tokenInfo getIDToken(buffer B, int i)
{
	// Find the identifier token. Return even if it is a keyword as an identifier
	if(i == 0)
	{
		showError();
		return NULL;
	}
	tokenInfo idToken = malloc(sizeof(tokenInfo));
	char* alphabet = malloc(i*sizeof(char));
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
		if(alphabet[0] >= '0' && alphbet[0] <= '9')
		{
			// Last letter of the alphabet. Return it right now.
			idToken->token_value = alphabet;
			free(alphabet);
			return idToken;
		}
		tokenInfo nextToken = getIDToken(B, --i);
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

tokenInfo getStringToken(buffer B, int i)
{
	// Find the string text.
	if(i == 0)
	{
		showError();
		return NULL;
	}
	tokenInfo stringToken = malloc(sizeof(tokenInfo));
	char* alphabet = malloc(i*sizeof(char));
	alphabet[0] = B->buff[B->fwdPointer];
	if(alphabet[0] < 'a' || alphabet[0] > 'z')
	{
		// Not an alphabet. End here.
		free(stringToken);
		free(alphabet);
		return NULL;
	}
	else
	{
		// It is an alphabet. Keep going on.
		++B->fwdPointer;
		tokenInfo nextToken = getFunctionToken(B, --i);
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

tokenInfo getNumberToken(buffer B)
{

}

tokenInfo makeSingleToken(buffer B)
{
	// Creates a token for a single character and returns it. Reduces a lot of repititive work.
	tokenInfo token = malloc(sizeof(tokenInfo));
	token->charNumber = B->curPointer;
	//Increment values of curPointer and fwdPointer;
	++B->curPointer;
	++B->fwdPointer;
	return token;
}

tokenInfo getNextToken(FILE *fp, buffer B)
{
	// Read the input from the buffer.
	int state = -1; // Stores the current state of the the scan.
	char alphabet = B->buff[B->fwdPointer];
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
		token->token_name = CP;
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
			B = B->nextBuffer;
			// Keep checking for how long comments go and return next token.
			return getNextToken(fp, B);
		}

		if(B->fwdPointer >= BUFFERSIZE)
		{
			// The comment has overflowed to next buffer. Finish reading comment and return the next token.
			getStream(fp, B->nextBuffer, BUFFERSIZE);
			// Set current buffer to next buffer
			B = B->nextBuffer;
			// Keep checking for how long comments go and return next token.
			return getEndCommentAndNextToken(fp, B);
		}
	}
	else if(a == '>' || a == '<')
	{
		// Check next token.
		++B->fwdPointer;
		tokenInfo token = malloc(sizeof(tokenInfo));
		token->charNumber = B->curPointer;
		tokenInfo nextToken = getNextToken(fp, B);
		if(nextToken->token_name == ASSIGNOP)
		{
			// Assign the correct value to this token.
			if(a == '>')
			{
				token->token_name = GE;
				B->curPointer = B->fwdPointer;
				free(nextToken);
				return token;
			}
			else
			{
				token->token_name = LE;
				B->curPointer = B->fwdPointer;
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
				B->curPointer = token->charNumber + 1;
				B->fwdPointer = B->curPointer;
				free(nextToken);
				return token;
			}
			else(a == '<')
			{
				token->token_name = LT;
				// Lost the next token. Need to generate that one again.
				B->curPointer = token->charNumber + 1;
				B->fwdPointer = B->curPointer;
				free(nextToken);
				return token;
			}
		}
	}
	else if(a == '=')
	{
		++B->fwdPointer;
		tokenInfo token = malloc(sizeof(tokenInfo));
		token->charNumber = B->curPointer;
		tokenInfo nextToken = getNextToken(fp, B);
		if(nextToken->token_name == ASSIGNOP)
		{
			// EQ token.
			token->token_name = EQ;
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
			B->curPointer = ++token->charNumber;
			B->fwdPointer = B->curPointer;
			free(nextToken);
			return token;
		}
	}
	else if(a == '.')
	{
		// One of the logical operators, possibly
		++B->fwdPointer;
		tokenInfo token = malloc(sizeof(tokenInfo));
		token->charNumber = B->curPointer;
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
					free(nextToken);
					return token;
				}
				else
				{
					// Something else. A Lexical error;
					showError();
					++B->fwdPointer;
					++B->curPointer;
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
					free(nextToken);
					return token;
				}
				else{
					// Lexical error.
					showError();
					++B->fwdPointer;
					++B->curPointer;
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
					free(nextToken);
					return token;
				}
				else{
					// Lexical error.
					showError();
					++B->fwdPointer;
					++B->curPointer;
					free(nextToken);
					free(token);
					return NULL;
				}
			}
			else
			{
				// Some other identifier. A Lexical error;
				showError();
				B->curPointer = ++token->charNumber;
				B->fwdPointer = B->curPointer;
				free(nextToken);
				free(token);
				return NULL;
			}
		}
		else
		{
			// A Lexical error;
			showError();
			B->curPointer = ++token->charNumber;
			B->fwdPointer = B->curPointer;
			free(nextToken);
			free(token);
			return NULL;
		}
	}
	else if(a == '_')
	{
		// A function identifier
		++B->fwdPointer;
		tokenInfo token = malloc(sizeof(tokenInfo));
		token->charNumber = B->curPointer;
		token->token_name = FUNID;
		char* funvalue = malloc((1+FUNSIZE)*sizeof(char));
		funvalue[0] = '_';
		if((B->buff[B->fwdPointer] >= 'a' && B->buff[B->fwdPointer] <= 'z') || (B->buff[B->fwdPointer] >= 'A' && B->buff[B->fwdPointer] <= 'Z'))
		{
			// Check for the function token.
			tokenInfo funToken = getFunctionToken(B, FUNSIZE);
			token->token_value = joinStrings(funvalue,funToken->token_value);
			B->curPointer = B->fwdPointer;
			free(funvalue);
			return token;
		}
		else
		{
			// Incorrect. Show error.
			showError();
			B->curPointer = B->fwdPointer;
			free(funvalue);
			free(token);
			return NULL;
		}
	}
	else if((a >= 'a' && a<= 'z') || (a >= 'A' && a<= 'Z'))
	{
		// An alphabet identifier
		++B->fwdPointer;
		tokenInfo token = malloc(sizeof(tokenInfo));
		token->charNumber = B->curPointer;
		token->token_name = ID;
		tokenInfo idToken = getIDToken(B);
		token->token_value = idToken->token_value;
		free(idToken);
		return token;
	}
	else if(a >= '0' && a<= '9')
	{
		// A number identifier
		++B->fwdPointer;
		tokenInfo token = malloc(sizeof(tokenInfo));
		token->charNumber = B->curPointer;
		tokenInfo numToken = getNumberToken(B);
		token->token_name = numToken->token_name;
		token->token_value = numToken->token_value;
		free(numToken);
		return token;
	}
	else if(a == '\"')
	{
		// String type.
		++B->fwdPointer;
		tokenInfo token = malloc(sizeof(tokenInfo));
		token->charNumber = B->curPointer;
		token->token_name = STRING;
		tokenInfo stringToken = getStringToken(B, IDSIZE);
		token->token_value = stringToken->token_value;
		free(stringToken);
		return token;
	}
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
	tokenInfo tokens = (tokenInfo)malloc(sizeof(tokenInfo));
	tokens->nextToken = NULL;
	tokenInfo head = tokens;	//Points to head token
	buffer firstBuff = (buffer)malloc(sizeof(buffer));
	// Initialise the first buffer stream
	getStream(fp, firstBuff, BUFFERSIZE);
	firstBuff->curPointer = 0;
	firstBuff->fwdPointer = 0;
	firstBuff->lineNumber = 1;
	buffer curBuff = firstBuff; // Current buffer being used.
	buffer secondBuff = (buffer)malloc(sizeof(buffer));
	secondBuff->curPointer = 0;
	secondBuff->fwdPointer = 0;
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
				curBuff = secondBuff;
			}
			else
			{
				// The second buffer was current.
				getStream(fp, firstBuff, BUFFERSIZE);
				curBuff = firstBuff;
			}
		}
		else
		{
			// correct token returned!
			tokens->lineNumber = curBuff->lineNumber;
			// Initialize the next token in the linked list.
			tokens->nextToken = (tokenInfo)malloc(sizeof(tokenInfo));
			// Set value of charNumber to current position of pointer
			charNumber = curBuff->curPointer;
			// Set tokens to point to next tokens.
			tokens = tokens->nextToken;
			tokens->nextToken = NULL;
		}
	} // Here's hopin this kind of works.
}