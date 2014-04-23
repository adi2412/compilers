/* Compilers Project Group 2
// Aditya Raisinghani 2011A7PS042P
// Utkarsh Verma 2011A7PS137P
// BITS Pilani, Pilani Campus
// Second semester 2014
*/

#ifndef SYMBOL
#define SYMBOL


typedef struct _STList *STList;

typedef struct _STable *STable;

typedef struct _identifier *identifier;

typedef enum {VAR,PARA,RETURN} idType;

typedef struct _matrixSizes *matrixSizes;

typedef struct _stringSizes *stringSizes;

struct _STList
{
	STList parentList;
	STList sisterList;
	STList childList;
	STable table;
	matrixSizes matrixTable;
	stringSizes stringTable;
	char* functionName;
	int scopeIdentifier;
	int startLineNumber;
	int startCharNumber;
	int endLineNumber;
	int endCharNumber;
};

struct _STable
{
	identifier data;
	STable nextEntry;
	int scope;
	int offset;
	int lineNumber;
	int charNumber;
	idType type;
	int scopeIdentifier; // Not sure if neeeded
};

/**
 * Separate structure to store string names and their sizes.
 */
struct _stringSizes
{
	char* stringName;
	int length;
	stringSizes nextEntry;
};

/**
 * Separate structure to store matrix names and their rows and columns.
 */
struct _matrixSizes
{
	char* matrixName;
	int rows;
	int columns;
	matrixSizes nextEntry;
};

/**
 * Stores the name of each identifier in symbol table.
 */
struct _identifier
{
	token symbol;
	char* value;
	token type;	
};

void checkNextStatementAndRead();
void createSymbolTables();
token convertType(token type);
void printSymbolTable(STList list);
STList generateSymbolTables(astTree astRoot);

#endif