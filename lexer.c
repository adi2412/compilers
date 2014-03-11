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
#include 'tokens.h'

#define BUFFERSIZE 1024
#define FUNSIZE 20 // Don't know the correct value currently UPDATE
#define IDSIZE 20 //UPDATE
#define NUMSIZE 20 //UPDATE

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

tokenInfo getNumberToken(buffer B)
{	
	//integers, and real number upto 2 decimal places supported
	tokenInfo numToken = malloc(sizeof(tokenInfo));
	char* alphabet = malloc(NUMSIZE*sizeof(char));
	int i=0;
	alphabet[i++] = B->buff[B->fwdPointer];


	while(((B->buff[B->fwdPointer] >= '0') && (B->buff[B->fwdPointer] <= '9')) && i< NUMSIZE)  //UPDATE:how to handle the number size limit?
	{
		alphabet[i++] = B->buff[++B->fwdPointer];	
		//++B->fwdPointer; //included inline above
	}

	//keep moving pointer ahead of numbers, till we reach . or some another character (error)
	if(B->buff[B->fwdPointer] == '.')
	{	
		if(B->buff[(B->fwdPointer)+1] >='0' && B->buff[(B->fwdPointer)+1] <='9') //+1 to ptr to peek ahead and see if nos ahead, append the . if true
		{
			alphabet[i++] = B->buff[++B->fwdPointer];
		}
				
		int maxDecimalPlace=2; int currentDecimalPlace=0;
		while(((B->buff[B->fwdPointer] >= '0') && (B->buff[B->fwdPointer] <= '9')) && i< NUMSIZE && currentDecimalPlace < maxDecimalPlace)  //UPDATE:how to handle the number size limit?
		{
			alphabet[i++] = B->buff[++B->fwdPointer];	
			++currentDecimalPlace;
		}
		if(currentDecimalPlace==2) //  B->buff[B->fwdPointer] == '.')
		{	//it is obv upto maxDecimalPlace only by code design
			numToken->token_name = "RNUM";
			numToken->token_value = alphabet; //CONSIDER:this will need atof conversion later.

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
		numToken->token_name = "NUM";
		free(alphabet);
		return numToken;	//CONSIDER: the fwdPointer has increasd. handle that?
	}


}

tokenInfo getNextToken(FILE *fp, buffer B)
{
	// Read the input from the buffer.
	int state = -1; // Stores the current state of the the scan.
	char alphabet = B->buff[B->fwdPointer];
	// Check first for the one character tokens.
	// TODO : implement strings.
	// Implement matrix type. Change the SQO check.
	if(a == '(')
	{
		tokenInfo token = malloc(sizeof(tokenInfo));
		token->token_name = OP;
		token->charNumber = B->curPointer;
		//Increment values of curPointer and fwdPointer;
		++B->curPointer;
		++B->fwdPointer;
		return token;
	}
	else if(a == ')')
	{
		tokenInfo token = malloc(sizeof(tokenInfo));
		token->token_name = CL;
		token->charNumber = B->curPointer;
		//Increment values of curPointer and fwdPointer;
		++B->curPointer;
		++B->fwdPointer;
		return token;
	}
	else if(a == '[')
	{
		tokenInfo token = malloc(sizeof(tokenInfo));
		token->token_name = SQO;
		token->charNumber = B->curPointer;
		//Increment values of curPointer and fwdPointer;
		++B->curPointer;
		++B->fwdPointer;
		return token;
	}
	else if(a == ']')
	{
		tokenInfo token = malloc(sizeof(tokenInfo));
		token->token_name = SQC;
		token->charNumber = B->curPointer;
		//Increment values of curPointer and fwdPointer;
		++B->curPointer;
		++B->fwdPointer;
		return token;
	}
	else if(a == ';')
	{
		tokenInfo token = malloc(sizeof(tokenInfo));
		token->token_name = SEMICOLON;
		token->charNumber = B->curPointer;
		//Increment values of curPointer and fwdPointer;
		++B->curPointer;
		++B->fwdPointer;
		return token;
	}
	else if(a == ',')
	{
		tokenInfo token = malloc(sizeof(tokenInfo));
		token->token_name = COMMA;
		token->charNumber = B->curPointer;
		//Increment values of curPointer and fwdPointer;
		++B->curPointer;
		++B->fwdPointer;
		return token;
	}
	else if(a == '+')
	{
		tokenInfo token = malloc(sizeof(tokenInfo));
		token->token_name = PLUS;
		token->charNumber = B->curPointer;
		//Increment values of curPointer and fwdPointer;
		++B->curPointer;
		++B->fwdPointer;
		return token;
	}
	else if(a == '-')
	{
		tokenInfo token = malloc(sizeof(tokenInfo));
		token->token_name = MINUS;
		token->charNumber = B->curPointer;
		//Increment values of curPointer and fwdPointer;
		++B->curPointer;
		++B->fwdPointer;
		return token;
	}
	else if(a == '*')
	{
		tokenInfo token = malloc(sizeof(tokenInfo));
		token->token_name = MUL;
		token->charNumber = B->curPointer;
		//Increment values of curPointer and fwdPointer;
		++B->curPointer;
		++B->fwdPointer;
		return token;
	}
	else if(a == '/')
	{
		tokenInfo token = malloc(sizeof(tokenInfo));
		token->token_name = DIV;
		token->charNumber = B->curPointer;
		//Increment values of curPointer and fwdPointer;
		++B->curPointer;
		++B->fwdPointer;
		return token;
	}
	else if(a == '@')
	{
		tokenInfo token = malloc(sizeof(tokenInfo));
		token->token_name = SIZE;
		token->charNumber = B->curPointer;
		//Increment values of curPointer and fwdPointer;
		++B->curPointer;
		++B->fwdPointer;
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
				return token;
			}
			else
			{
				token->token_name = LE;
				B->curPointer = B->fwdPointer;
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
				return token;
			}
			else(a == '<')
			{
				token->token_name = LT;
				// Lost the next token. Need to generate that one again.
				B->curPointer = token->charNumber + 1;
				B->fwdPointer = B->curPointer;
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
				return token;
			}
			else
			{
				// Not NE token. An error
				// TODO: Decide error functions
				showError();
				++B->fwdPointer;
				++B->curPointer;
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
					return token;
				}
				else
				{
					// Something else. A Lexical error;
					showError();
					++B->fwdPointer;
					++B->curPointer;
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
					return token;
				}
				else{
					// Lexical error.
					showError();
					++B->fwdPointer;
					++B->curPointer;
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
					return token;
				}
				else{
					// Lexical error.
					showError();
					++B->fwdPointer;
					++B->curPointer;
					return NULL;
				}
			}
			else
			{
				// Some other identifier. A Lexical error;
				showError();
				B->curPointer = ++token->charNumber;
				B->fwdPointer = B->curPointer;
				return NULL;
			}
		}
		else
		{
			// A Lexical error;
			showError();
			B->curPointer = ++token->charNumber;
			B->fwdPointer = B->curPointer;
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
		if((B->buff[B->fwdPointer] >= 'a' && B->buff[B->fwdPointer] <= 'z') || (B->buff[B->fwdPointer] >= 'A' && B->buff[B->fwdPointer] <= 'Z'))
		{
			// Check for the function token.
			tokenInfo funToken = getFunctionToken(B, FUNSIZE);
			token->token_value = funToken->token_value;
			B->curPointer = B->fwdPointer;
			return token;
		}
		else
		{
			// Incorrect. Show error.
			showError();
			B->curPointer = B->fwdPointer;
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
		tokenInfo idToken = getIDToken(B); //!MODIFY! , need i value passed.
		token->token_value = idToken->token_value;
		return token;
	}
	else if(a >= '0' && a<= '9')
	{
		// A number identifier
		++B->fwdPointer;
		tokenInfo token = malloc(sizeof(tokenInfo));
		token->charNumber = B->curPointer;
		tokenInfo numToken = getNumberToken(B);
		token->token_name = numToken->token_name; //whether RealNum or IntNum.
		token->token_value = numToken->token_value;
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