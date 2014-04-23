/* Compilers Project Group 2
// Aditya Raisinghani 2011A7PS042P
// Utkarsh Verma 2011A7PS137P
// BITS Pilani, Pilani Campus
// Second semester 2014
*/

// Contains functions for parsing

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tokens.h"
#include "lexer.h"
#include "parserDef.h"
#include "first_follow_gen.h"
#include "parser.h"
#include "sem_parser.h"
#include "ast.h"
#include "symbol_table.h"
#include "type_extractor.h"
#include "semantic_analyzer.h"

astTree currentASTNode;
STList stList;
STList headList;
STable currentEntry;
matrixSizes currentMatrix;
stringSizes currentString;
// STable headEntry;
int curScope;
int scopeIdentifier;

void STError(int num,char* name,int lineNum,int charNum)
{
	printf("\x1b[31mSemantic Error 2: Identifier \x1b[37m\x1b[1m%s\x1b[31m must be declared before use on line \x1b[37m\x1b[1m%d:%d\n\x1b[0m",name,lineNum,charNum);
}

/*
// Checks whether a specific id already exists in 
// symbol table
*/
int doesExistInSymbolTable(char* name)
{
	STable entry = stList->table;
	STList readList = stList;
	if(entry->data == NULL)
	{
		if(readList->parentList == NULL)
			return 0;
		entry = readList->parentList->table;
		readList = readList->parentList;
	}
	while(strcmp(entry->data->value, name))
	{
		entry = entry->nextEntry;
		if(entry->data == NULL)
		{
			// All entries in the current scope over. Check parent scope.
			if(readList->parentList == NULL)
				return 0;
			entry = readList->parentList->table;
			readList = readList->parentList;
		}
	}
	return 1;
}

/*
// Converts ID, and data to one specific standard token
*/
token convertType(token type)
{
	if(type == INT)
		return NUM;
	else if(type == REAL)
		return RNUM;
	else if(type == STRING)
		return STR;
	else if(type == MATRIX)
		return MATRIX;
	return INVALID;
}

/*
// Gets the type of a variable from the symbol table.
*/
token checkTypeInFormedSymbolTable(char* name, int lineNum,int charNum)
{
	STable entry = stList->table;
	STList readList = stList;
	if(entry->data == NULL)
	{
		if(readList->parentList == NULL)
		{
			STError(0,name,lineNum,charNum);
			return INVALID;
		}
		entry = readList->parentList->table;
		readList = readList->parentList;
	}
	while(strcmp(entry->data->value,name))
	{
		entry = entry->nextEntry;
		if(entry->data == NULL)
		{
			// All entries in current scope over. Check above scope.
			if(readList->parentList == NULL)
			{
				STError(0,name,lineNum,charNum);
				entry = NULL;
				return INVALID;
			}
			entry = readList->parentList->table;
			readList = readList->parentList;
		}
	}
	return convertType(entry->data->type);
}

void initialiseSymbolTable()
{
	stList->parentList = NULL;
	stList->sisterList = NULL;
	stList->childList = NULL;
	stList->functionName = "main";
	stList->startCharNumber = 0;
	stList->startLineNumber = 0;
	stList->scopeIdentifier = scopeIdentifier;
	STable headEntry;
	currentEntry = malloc(sizeof(struct _STable));
	currentEntry->nextEntry = NULL;
	currentEntry->data = NULL;
	// stList->table = currentEntry;
	headEntry = currentEntry;
	stList->table = headEntry;
	currentMatrix = malloc(sizeof(struct _matrixSizes));
	currentMatrix->nextEntry = NULL;
	matrixSizes headMatrix;
	headMatrix = currentMatrix;
	stList->matrixTable = headMatrix;
	currentString = malloc(sizeof(struct _stringSizes));
	currentString->stringName = NULL;
	currentString->nextEntry = NULL;
	stringSizes headString;
	headString = currentString;
	stList->stringTable = headString;
	// currentEntry = headEntry;
	// currentEntry->nextEntry = malloc(sizeof(struct _STable));
	headList = stList;
}

/*
// Adds a new variable to the symbol table.
*/
void addToCurrentSymbolTable(astTree data, token type,int idType)
{
	if(doesExistInSymbolTable(data->data.token_data))
	{
		// Same name already declared before.
		printf("\x1b[31mSemantic Error 2: Identifier \x1b[37m\x1b[1m%s\x1b[31m has been declared twice on line \x1b[37m\x1b[1m%d:%d\n\x1b[0m",data->data.token_data,data->data.lineNumber,data->data.charNumber);
		return;
	}
	currentEntry->scope = curScope;
	currentEntry->offset = 0;
	currentEntry->lineNumber = data->data.lineNumber;
	currentEntry->charNumber = data->data.charNumber;
	currentEntry->type = idType;
	currentEntry->scopeIdentifier = scopeIdentifier; //the identifier value.
	currentEntry->data = malloc(sizeof(struct _identifier));
	currentEntry->data->symbol = data->data.tokValue; // Is there a need for this?
	currentEntry->data->value = data->data.token_data;
	currentEntry->data->type = type;
	currentEntry->nextEntry = malloc(sizeof(struct _STable));
	currentEntry = currentEntry->nextEntry;
	currentEntry->nextEntry = NULL;
	currentEntry->data = NULL;
}

/*
// Add all the variables on the left hand side of
// assignment type2 statement to symbol table.
*/
void findIdentifiers()
{
	currentASTNode = currentASTNode->childNode;
	astTree typeNode = currentASTNode->childNode;
	sem typeValue = typeNode->childNode->element;
	token type = typeValue.leafName;
	astTree varNodes = typeNode->sisterNode;
	while(varNodes != NULL)
	{
		astTree dataNode = varNodes->childNode;
		addToCurrentSymbolTable(dataNode,type,0);
		varNodes = varNodes->childNode->sisterNode;
		if(varNodes->ruleNum == 33)
		{
			// Identifiers are over.
			break;
		}
		else
		{
			varNodes = varNodes->childNode;
		}
	}
	currentASTNode = currentASTNode->parentNode;
	return;
}

/*
// Finds all the parameters and return values
// of a function and adds them to the symbol table
*/
void findParameters(int idType)
{
	currentASTNode = currentASTNode->childNode;
	astTree typeNode = currentASTNode->childNode;
	sem typeValue = typeNode->element;
	token type = typeValue.leafName;
	astTree varNodes = currentASTNode->sisterNode;
	astTree dataNode = varNodes;
	addToCurrentSymbolTable(dataNode,type,idType);
	varNodes = varNodes->sisterNode;
	while(varNodes->ruleNum != 20)
	{
		typeNode = varNodes->childNode->childNode;
		typeValue = typeNode->childNode->element;
		type = typeValue.leafName;
		varNodes = typeNode->sisterNode;
		dataNode = varNodes;
		addToCurrentSymbolTable(dataNode,type,idType);
		varNodes = varNodes->sisterNode;
	}
	currentASTNode = currentASTNode->parentNode;
	return;
}

/*
// Checks if an assignment statement has a matrix
// assignment and if so adds its rows and columns
// to symbol table.
*/
void findMatrixAssignment()
{
	astTree idNode = currentASTNode->childNode->childNode->childNode;
	token type = checkTypeInFormedSymbolTable(idNode->data.token_data,idNode->data.lineNumber,idNode->data.charNumber);
	if(type == MATRIX)
	{
		// It is a matrix.
		astTree rhsNode = currentASTNode->childNode->childNode->sisterNode;
		if(rhsNode->ruleNum == 26)
		{
			astTree matrixNode = rhsNode->childNode->childNode->childNode->childNode;
			if(matrixNode->ruleNum == 69)
			{
				// It is a matrix assignment.
				astTree rows = matrixNode->childNode->childNode;
				int numRows = 0;
				int numCols = -1;
				int newCol = 0;
				while(rows!= NULL)
				{
					numRows++;
					astTree row = rows->childNode;
					newCol = 0;
					while(row!= NULL)
					{
						newCol++;
						row = row->childNode->sisterNode;
						if(row->ruleNum == 76)
						{
							row = NULL;
						}
					}
					rows = rows->childNode->sisterNode;
					if(rows->ruleNum == 73)
					{
						rows = NULL;
					}
					else
					{
						rows = rows->childNode;
					}
					if(newCol != numCols && numCols != -1)
					{
						// Rows have different number of columns.
						printf("\x1b[31mSemantic Error 2: Rows have different number of columns for matrix %s on line \x1b[37m\x1b[1m%d:%d\n\x1b[0m",idNode->data.token_data,idNode->data.lineNumber,idNode->data.charNumber);
						exit(0);
					}
					else
					{
						numCols = newCol;
					}
				}
				currentMatrix->matrixName = idNode->data.token_data;
				currentMatrix->rows = numRows;
				currentMatrix->columns = numCols;
				currentMatrix->nextEntry = malloc(sizeof(struct _matrixSizes));
				currentMatrix = currentMatrix->nextEntry;
				currentMatrix->nextEntry = NULL;
			}
		}
	}
	else if(type == STR)
	{
		// It is a string.
		astTree rhsNode = currentASTNode->childNode->childNode->sisterNode;
		if(rhsNode->ruleNum == 26)
		{
			astTree stringNode = rhsNode->childNode->childNode->childNode->childNode;
			if(stringNode->ruleNum == 68)
			{
				if(idNode->data.token_data != NULL)
				{
					currentString->stringName = idNode->data.token_data;
					currentString->length = strlen(stringNode->childNode->data.token_data);
					currentString->nextEntry = malloc(sizeof(struct _stringSizes));
					currentString = currentString->nextEntry;
					currentString->nextEntry = NULL;
					currentString->stringName = NULL;
				}
			}
		}
	}
}

/*
// Adds a symbol table for a if part as a child to the
// the parent function symbol table.
*/
void addSymbolTableToList()
{
	if(stList->childList != NULL)
	{
		// Previous a child list exists. Create a new sister list.
		STList parentList = stList;
		stList = stList->childList;
		while(stList->sisterList != NULL)
		{
			stList = stList->sisterList;
		}
		stList->sisterList = malloc(sizeof(struct _STList));
		stList = stList->sisterList;
		stList->parentList = parentList;
		stList->sisterList = NULL;
		stList->childList = NULL;
		stList->functionName = "if";
		stList->scopeIdentifier = scopeIdentifier;
		astTree tempNode = currentASTNode->childNode->childNode->sisterNode;
		while(tempNode->data.flag)
			tempNode = tempNode->childNode;
		stList->startLineNumber = tempNode->data.lineNumber;
		stList->startCharNumber = tempNode->data.charNumber;
		STable headEntry = malloc(sizeof(struct _STable));
		headEntry->data = NULL;
		headEntry->nextEntry = NULL;
		stList->table = headEntry;
		currentEntry = headEntry;
		matrixSizes headMatrix = malloc(sizeof(struct _matrixSizes));
		headMatrix->nextEntry = NULL;
		stList->matrixTable = headMatrix;
		currentMatrix = headMatrix;
		stringSizes headString = malloc(sizeof(struct _stringSizes));
		headString->stringName = NULL;
		headString->nextEntry = NULL;
		stList->stringTable = headString;
		currentString = headString;
		// Go into the block and find the variables.
		currentASTNode = currentASTNode->childNode->childNode->sisterNode; //stmt rule.
		// createSymbolTables();
	}
	else
	{
		// No child exists. Create one.
		stList->childList = malloc(sizeof(struct _STList));
		stList->childList->parentList = stList;
		stList = stList->childList;
		stList->sisterList = NULL;
		stList->childList = NULL;
		stList->functionName = "if";
		matrixSizes headMatrix = malloc(sizeof(struct _matrixSizes));
		headMatrix->nextEntry = NULL;
		stList->matrixTable = headMatrix;
		currentMatrix = headMatrix;
		stList->scopeIdentifier = scopeIdentifier;
		astTree tempNode = currentASTNode->childNode->childNode->sisterNode;
		while(tempNode->data.flag)
			tempNode = tempNode->childNode;
		stList->startLineNumber = tempNode->data.lineNumber;
		stList->startCharNumber = tempNode->data.charNumber;
		STable headEntry = malloc(sizeof(struct _STable));
		headEntry->data = NULL;
		headEntry->nextEntry = NULL;
		stList->table = headEntry;
		currentEntry = headEntry;
		stringSizes headString = malloc(sizeof(struct _stringSizes));
		headString->stringName = NULL;
		headString->nextEntry = NULL;
		stList->stringTable = headString;
		currentString = headString;
		// Go into the block and find the variables.
		currentASTNode = currentASTNode->childNode->childNode->sisterNode; //stmt rule.
		// createSymbolTables();
	}
}

/*
// Separate function to add symbol table for else part
// as child to the parent function symbol table.
*/
void addSymbolTableForElsePartToList()
{
	if(stList->childList != NULL)
	{
		// Previous a child list exists. Create a new sister list.
		STList parentList = stList;
		stList = stList->childList;
		while(stList->sisterList != NULL)
		{
			stList = stList->sisterList;
		}
		stList->sisterList = malloc(sizeof(struct _STList));
		stList = stList->sisterList;
		stList->parentList = parentList;
	}
	else
	{
		// No child exists. Create one.
		stList->childList = malloc(sizeof(struct _STList));
		stList->childList->parentList = stList;
		stList = stList->childList;
	}
	stList->sisterList = NULL;
	stList->childList = NULL;
	stList->functionName = "else";
	matrixSizes headMatrix = malloc(sizeof(struct _matrixSizes));
	headMatrix->nextEntry = NULL;
	stList->matrixTable = headMatrix;
	currentMatrix = headMatrix;
	stringSizes headString = malloc(sizeof(struct _stringSizes));
	headString->stringName = NULL;
	headString->nextEntry = NULL;
	stList->stringTable = headString;
	currentString = headString;
	stList->scopeIdentifier = scopeIdentifier;
	astTree tempNode = currentASTNode->childNode;
	while(tempNode->data.flag)
		tempNode = tempNode->childNode;
	stList->startLineNumber = tempNode->data.lineNumber;
	stList->startCharNumber = tempNode->data.charNumber;
	STable headEntry = malloc(sizeof(struct _STable));
	headEntry->data = NULL;
	headEntry->nextEntry = NULL;
	stList->table = headEntry;
	currentEntry = headEntry;
	// Go into the block and find the variables.
	currentASTNode = currentASTNode->childNode; 
}

/*
// Adds symbol table or a new function as child to the
// parent function symbol table.
*/
void addSymbolTableForFunctionToList()
{
	if(stList->childList != NULL)
	{
		// Previous a child list exists. Create a new sister list.
		STList parentList = stList;
		stList = stList->childList;

		while(stList->sisterList != NULL)
		{
			stList = stList->sisterList;
		}
		stList->sisterList = malloc(sizeof(struct _STList));
		stList = stList->sisterList;
		stList->parentList = parentList;
	}
	else
	{
		// No child exists. Create one.
		stList->childList = malloc(sizeof(struct _STList));
		stList->childList->parentList = stList;
		stList = stList->childList;
	}
	stList->sisterList = NULL;
	stList->childList = NULL;
	matrixSizes headMatrix = malloc(sizeof(struct _matrixSizes));
	headMatrix->nextEntry = NULL;
	stList->matrixTable = headMatrix;
	currentMatrix = headMatrix;
	stringSizes headString = malloc(sizeof(struct _stringSizes));
	headString->stringName = NULL;
	headString->nextEntry = NULL;
	stList->stringTable = headString;
	currentString = headString;
	stList->scopeIdentifier = scopeIdentifier;
	STable headEntry = malloc(sizeof(struct _STable));
	headEntry->data = NULL;
	headEntry->nextEntry = NULL;
	stList->table = headEntry;
	currentEntry = headEntry;
	currentASTNode = currentASTNode->childNode;
	// TODO: Add the return identifiers to the symbol table.
	findParameters(2);
	// Add the function name to the STList
	currentASTNode = currentASTNode->sisterNode;
	char* functionName = currentASTNode->data.token_data;
	stList->functionName = functionName;
	stList->startLineNumber = currentASTNode->data.lineNumber;
	stList->startCharNumber = currentASTNode->data.charNumber;
	// TODO: Add the parameters to the symbol table.
	currentASTNode = currentASTNode->sisterNode;
	findParameters(1);
	// Go into the function and find the variables.
	currentASTNode = currentASTNode->sisterNode->childNode;
	checkNextStatementAndRead();
}

/*
// Pops symbol table of current function from current 
// symbol list ince it is
// over and retuns to symbol table of parent function.
*/
void popSymbolTable()
{
	if(stList->parentList == NULL)
		return;
	stList = stList->parentList;
	STable entry = stList->table;
	while(entry->data != NULL)
		entry = entry->nextEntry;
	currentEntry = entry;
	curScope--;
}

/*
// Increases the scope value when a new if/else
// statement has been seen.
*/
void goToNewScope()
{
	curScope++;
	scopeIdentifier++;
	addSymbolTableToList();
}

/*
// Increases the scope value when a new function declaration
// statement has been seen.
*/
void goToNewFunctionScope()
{
	curScope++;
	scopeIdentifier++;
	addSymbolTableForFunctionToList();
}

/*
// Finds the next statement to compute.
*/
void findNextStatement()
{
	if(currentASTNode == NULL)
		return;
	if(currentASTNode->ruleNum == 6)
	{
		// It is a nullable.
		astTree tempNode = currentASTNode; // to find line and char numbers of end of block.
		tempNode = tempNode->parentNode->childNode; // stmtOrFunctionDef.
		while(tempNode->data.flag)
			tempNode = tempNode->childNode;
		stList->endLineNumber = tempNode->data.lineNumber;
		stList->endCharNumber = tempNode->data.charNumber;
		if(curScope == 0)
		{
			// We are in the main function scope and hence the end of the program. End here.
			currentASTNode = NULL;
			return;
		}
		else
		{
			// We are in some inner function and hence there might be other statements in the above function. Find it.
			while(currentASTNode->element.nontermValue != functionDef)
			{
				currentASTNode = currentASTNode->parentNode;
			}
			// We found the functionDef node.
			popSymbolTable();
			currentASTNode = currentASTNode->parentNode->sisterNode; //stmtsAndFunctionDef_type1
			findNextStatement();
			return;
		}
	}
	else
	{
		// More statements exist.
		currentASTNode = currentASTNode->childNode->childNode; //stmtorFunctionDef
		return;
	}
}

/*
// Checks whether a functionDef or stmt is occurring
// and calls appropriate functions.
*/
void checkNextStatementAndRead()
{
	// This function should always receive currentASTNode pointing at stmtOrFunctionDef
	if(currentASTNode != NULL)
	while(currentASTNode != NULL)
	{
		if(currentASTNode->ruleNum == 3)
		{
			currentASTNode = currentASTNode->childNode;
			createSymbolTables();
		}
		else
		{
			// It is a functionDef statement.
			currentASTNode = currentASTNode->childNode;
			goToNewFunctionScope();
			// popSymbolTable();
			findNextStatement();
		}
	}
	if(currentASTNode == NULL)
		return;
}

/*
// Separate function to create symbol table for statements
// within the if block.
*/
void createSymbolTablesForIfBlock()
{
	while(1)
	{
		int rule = currentASTNode->ruleNum - 7;
		switch(rule)
		{
			case 0: findIdentifiers(); // declaration statement.
			case 1: findMatrixAssignment();break; // Assignment stmt type1
			case 2: break; // Assignment stmt type2
			case 3: goToNewScope(); // if stmt
			case 4: break; // IO Stmt.
			case 5: break; // function call statement.
			default: break; // Some other rule number.
		}
		if(currentASTNode->sisterNode->ruleNum == 39)
		{
			astTree tempNode = currentASTNode;
			while(tempNode->data.flag)
				tempNode = tempNode->childNode;
			stList->endLineNumber = tempNode->data.lineNumber;
			stList->endCharNumber = tempNode->data.charNumber;
			return;
		}
		else
		{
			currentASTNode = currentASTNode->sisterNode->childNode;
		}
	}
	if(currentASTNode == NULL)
		return;
}

/*
// Finds the ifstmt rule once all the statements within
// if block are over in order to go back to parent function.
*/
void findIfStatement()
{
	if(currentASTNode == NULL)
		return;
	while(currentASTNode->ruleNum != 35)
	{
		currentASTNode = currentASTNode->parentNode;
		if(currentASTNode == NULL)
		{
			fprintf(stderr, "Reached NULL while finding if.\n");
			exit(0);
		}
	}
}

/*
// Create symbol table based on different statements
// in a function scope.
*/
void createSymbolTables()
{
	int rule = currentASTNode->ruleNum - 7;
	switch(rule)
	{
		case 0: findIdentifiers();break; // declaration statement.
		case 1: findMatrixAssignment();break; // Assignment stmt type1
		case 2: break; // Assignment stmt type2
		case 3: goToNewScope();break; // if stmt
		case 4: break; // IO Stmt.
		case 5: break; // function call statement.
		default: break; // Some other rule number.
	}
	if(currentASTNode->parentNode->ruleNum == 3)
	{
		currentASTNode = currentASTNode->parentNode->sisterNode; // stmtsAndFunctionDef_type1
		if(currentASTNode == NULL)
			return;
		findNextStatement();
		checkNextStatementAndRead();
	}
	else if(currentASTNode->parentNode->ruleNum == 35)
	{
		// At the stmt rule in the if block.
		createSymbolTablesForIfBlock();
		findIfStatement();
		if(currentASTNode == NULL)
			return;
		currentASTNode = currentASTNode->childNode->sisterNode->sisterNode->sisterNode; // elsePart.
		if(currentASTNode->ruleNum == 37)
		{
			// else Part does not exist.
			popSymbolTable();
		}
		else
		{
			// else part exists too.
			popSymbolTable();
			curScope++;
			scopeIdentifier++;
			addSymbolTableForElsePartToList();
			createSymbolTablesForIfBlock();
			findIfStatement();
			popSymbolTable();
			if(currentASTNode == NULL)
				return;
			currentASTNode = currentASTNode->parentNode->parentNode->sisterNode; //stmtsAndFunctionDefs_type1
			findNextStatement();
			checkNextStatementAndRead();
		}
	}
	if(currentASTNode == NULL)
		return;
	while(currentASTNode->ruleNum != 3 || currentASTNode->ruleNum != 4)
		currentASTNode = currentASTNode->parentNode;
	return;
}

/*
// Main function called by driver to generate symbol table
// for the code using the AST.
*/
STList generateSymbolTables(astTree astRoot)
{
	stList = malloc(sizeof(struct _STList));
	curScope = 0;
	scopeIdentifier = 0;
	initialiseSymbolTable();
	currentASTNode = astRoot;
	currentASTNode = currentASTNode->childNode->childNode; // stmtOrFunctionDef rule.
	checkNextStatementAndRead();
	return headList;
}