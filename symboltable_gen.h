//symboltable_gen.h header
/* Compilers Project Group 2
// Aditya Raisinghani 2011A7PS042P
// Utkarsh Verma 2011A7PS137P
// BITS Pilani, Pilani Campus
// Second semester 2014
*/

#ifndef SMBT
#define SMBT

#include "tokens.h"
/*
typedef enum _symbol
{

	INT,
	REAL,
	STRING,
	MATRIX,

	
} Symbol;
*/
typedef struct _tableEntry TableEntry;
typedef struct _tableEntry
{
	char* s;
	token sym;
	TableEntry* next;
} TableEntry;

typedef struct _tableEntry* Table;
typedef Table* prevTable;

//environment

typedef struct _env
{
	Table table;
	prevTable prev;
} Env;

typedef Env* EnvRef;



#endif