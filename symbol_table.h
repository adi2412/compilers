#ifndef SYMBOL
#define SYMBOL


typedef struct _STList *STList;

typedef struct _STable *STable;

typedef struct _identifier *identifier;

typedef enum {VAR,PARA,RETURN} idType;

typedef struct _matrixSizes *matrixSizes;

struct _STList
{
	STList parentList;
	STList sisterList;
	STList childList;
	STable table;
	matrixSizes matrixTable;
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

struct _matrixSizes
{
	char* matrixName;
	int rows;
	int columns;
	matrixSizes nextEntry;
};

struct _identifier
{
	token symbol;
	char* value;
	token type;	
};

void checkNextStatementAndRead();
void createSymbolTables();
STList generateSymbolTables(astTree astRoot);

#endif