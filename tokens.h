/* Compilers Project Group 2
// Aditya Raisinghani 2011A7PS042P
// Utkarsh Verma 2011A7PS137P
// BITS Pilani, Pilani Campus
// Second semester 2014
*/

// Contains the definions used for tokens.

// All the possible tokens enumerated.
typedef enum tokens{
	ASSIGNOP,
	COMMENT,
	FUNID,
	ID,
	NUM,
	RNUM,
	STR,
	END,
	INT,
	REAL,
	STRING,
	MATRIX,
	MAIN,
	SQO,
	SQC,
	OP,
	CL,
	SEMICOLON,
	COMMA,
	IF,
	ELSE,
	ENDIF,
	READ,
	PRINT,
	PLUS,
	MINUS,
	MUL,
	DIV,
	SIZE,
	AND,
	OR,
	NOT,
	LT,
	LE,
	EQ,
	GT,
	LE,
	NE
} token;

/*
// Types of attributes of a token, like its scoping in case of variables 
// or parameters for a function
*/

typedef struct _attributes
{
	char* attr_name;
	char* attr_value;
	attributes *nextAttribute;
} attributes;

typedef struct _tokenInfo
{
	token token_name;
	char* token_value;	// The actual name of the token. For eg, abc is the value of a token ID
	attributes *attr;		// List of all possible attributes for a token
	tokenInfo nextToken;
	int lineNumber;
	int charNumber;
	/* data */
} *tokenInfo;