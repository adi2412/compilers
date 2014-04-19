#ifndef SYMBOL
#define SYMBOL


typedef struct _STList *STList;

typedef struct _STable *STable;

typedef struct _identifier *identifier;

typedef enum {VAR,PARA,RETURN} idType;

struct _STList
{
	STList parentList;
	STList sisterList;
	STList childList;
	STable table;
	int identifier;
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
	// int scopeIdentifier; // Not sure if neeeded
};

struct _identifier
{
	token symbol;
	char* value;
	token type;	
};

void checkNextStatementAndRead();
void createSymbolTables();
int generateSymbolTables(astTree astRoot);

#endif