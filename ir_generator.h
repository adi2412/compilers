#ifndef IRGEN
#define IRGEN

typedef struct _IRTable *IRTable;

typedef struct _IRItem *IRItem;

typedef enum {INTEGER,FLOAT,STRINGENUM,MAT} typeVar;

typedef enum {ADD,SUB,MULTIPLY,DIVIDE,MOV,JMP} oper;

struct _IRTable
{
	int lineNum;
	oper op;
	IRItem arg1;
	IRItem arg2;
	IRItem result;
	IRTable nextEntry;
};

struct _IRItem
{
	char* entry;
	char* temp_name;
	int value;
	float floatValue;
	char* strValue;
	typeVar typeName;
	flag isSize;
	int rowNum;
	int colNum;
	flag jumpStmt;
	int jumpOffset;
};

#endif