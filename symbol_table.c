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
#include "symbol_table.h"

astTree currentASTNode;
STList stList;
STList headList;
STable currentEntry;
// STable headEntry;
int curScope;
int scopeIdentifier;

void initialiseSymbolTable()
{
	stList->parentList = NULL;
	stList->sisterList = NULL;
	stList->childList = NULL;
	STable headEntry;
	currentEntry = malloc(sizeof(struct _STable));
	currentEntry->nextEntry = NULL;
	currentEntry->data = NULL;
	stList->table = currentEntry;
	// currentEntry = headEntry;
	// currentEntry->nextEntry = malloc(sizeof(struct _STable));
	headList = stList;
}

void addToCurrentSymbolTable(astTree data, token type)
{
	currentEntry->scope = curScope;
	currentEntry->offset = 0;
	currentEntry->lineNumber = data->data.lineNumber;
	currentEntry->charNumber = data->data.charNumber;
	currentEntry->data = malloc(sizeof(struct _identifier));
	currentEntry->data->symbol = data->data.tokValue; // Is there a need for this?
	currentEntry->data->value = data->data.token_data;
	printf("Adding data: %s\n",currentEntry->data->value);
	currentEntry->data->type = type;
	currentEntry->nextEntry = malloc(sizeof(struct _STable));
	currentEntry = currentEntry->nextEntry;
	currentEntry->nextEntry = NULL;
	currentEntry->data = NULL;
}

void findIdentifiers()
{
	currentASTNode = currentASTNode->childNode;
	fprintf(stderr, "At find Identifier, number is %d\n", currentASTNode->element.nontermValue);
	astTree typeNode = currentASTNode->childNode;
	sem typeValue = typeNode->element;
	token type = typeValue.leafName;
	astTree varNodes = typeNode->sisterNode;
	while(varNodes != NULL)
	{
		astTree dataNode = varNodes->childNode;
		addToCurrentSymbolTable(dataNode,type);
		varNodes = varNodes->childNode->sisterNode;
		if(varNodes->ruleNum == 24)
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
	fprintf(stderr, "Leaving find Identifier, number is %d\n", currentASTNode->element.nontermValue);
	return;
}

void addSymbolTableToList()
{
	if(stList->childList != NULL)
	{
		// Previous a child list exists. Create a new sister list.
		STList parentList = stList;
		stList = stList->childList;
		while(stList != NULL)
		{
			stList = stList->sisterList;
		}
		stList = malloc(sizeof(struct _STList));
		stList->parentList = parentList;
		stList->sisterList = NULL;
		stList->childList = NULL;
		STable headEntry = malloc(sizeof(struct _STable));
		headEntry->data = NULL;
		headEntry->nextEntry = NULL;
		stList->table = headEntry;
		currentEntry = headEntry;
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
		STable headEntry = malloc(sizeof(struct _STable));
		headEntry->data = NULL;
		headEntry->nextEntry = NULL;
		stList->table = headEntry;
		currentEntry = headEntry;
		// Go into the block and find the variables.
		currentASTNode = currentASTNode->childNode->childNode->sisterNode; //stmt rule.
		// createSymbolTables();
	}
}

void addSymbolTableForElsePartToList()
{
	if(stList->childList != NULL)
	{
		// Previous a child list exists. Create a new sister list.
		STList parentList = stList;
		stList = stList->childList;
		while(stList != NULL)
		{
			stList = stList->sisterList;
		}
		stList = malloc(sizeof(struct _STList));
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
	STable headEntry = malloc(sizeof(struct _STable));
	headEntry->data = NULL;
	headEntry->nextEntry = NULL;
	stList->table = headEntry;
	currentEntry = headEntry;
	// Go into the block and find the variables.
	currentASTNode = currentASTNode->childNode; 
}

void addSymbolTableForFunctionToList()
{
	if(stList->childList != NULL)
	{
		// Previous a child list exists. Create a new sister list.
		STList parentList = stList;
		stList = stList->childList;

		while(stList != NULL)
		{
			stList = stList->sisterList;
		}
		stList = malloc(sizeof(struct _STList));
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
	STable headEntry = malloc(sizeof(struct _STable));
	headEntry->data = NULL;
	headEntry->nextEntry = NULL;
	stList->table = headEntry;
	currentEntry = headEntry;
	fprintf(stderr,"%d\n",currentASTNode->element.nontermValue);
	currentASTNode = currentASTNode->childNode;
	// TODO: Add the return identifiers to the symbol table.

	// TODO: Add the parameters to the symbol table.
	fprintf(stderr,"%d\n",currentASTNode->element.nontermValue);
	currentASTNode = currentASTNode->sisterNode->sisterNode;
	fprintf(stderr,"Fine till here\n");
	// Go into the function and find the variables.
	currentASTNode = currentASTNode->sisterNode->childNode;
	fprintf(stderr,"Fine till here 2\n");
	checkNextStatementAndRead();
	// if(currentASTNode.ruleNum == 3)
	// {
	// 	currentASTNode = currentASTNode->childNode;
	// 	createSymbolTables();
	// }
	// else
	// {
	// 	// It is a functionDef statement.
	// 	currentASTNode = currentASTNode->childNode;
	// 	goToNewFunctionScope();
	// }
	// createSymbolTables();
}

void popSymbolTable()
{
	stList = stList->parentList;
	curScope--;
}

void goToNewScope()
{
	curScope++;
	scopeIdentifier++;
	addSymbolTableToList();
}

void goToNewFunctionScope()
{
	curScope++;
	scopeIdentifier++;
	addSymbolTableForFunctionToList();
}

void findNextStatement()
{
	if(currentASTNode == NULL)
		return;
	if(currentASTNode->ruleNum == 6)
	{
		// It is a nullable.
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

void checkNextStatementAndRead()
{
	// This function should always receive currentASTNode pointing at stmtOrFunctionDef
	if(currentASTNode != NULL)
		fprintf(stderr, "Recieved at %d\n", currentASTNode->element.nontermValue);
	while(currentASTNode != NULL)
	{
		if(currentASTNode->ruleNum == 3)
		{
			fprintf(stderr, "Chossing this\n");
			currentASTNode = currentASTNode->childNode;
			createSymbolTables();
		}
		else
		{
			// It is a functionDef statement.
			currentASTNode = currentASTNode->childNode;
			goToNewFunctionScope();
			popSymbolTable();
			findNextStatement();
		}
	}
	if(currentASTNode == NULL)
		return;
}

void createSymbolTablesForIfBlock()
{
	while(1)
	{
		int rule = currentASTNode->ruleNum - 7;
		switch(rule)
		{
			case 0: findIdentifiers(); // declaration statement.
			case 1: break; // Assignment stmt type1
			case 2: break; // Assignment stmt type2
			case 3: goToNewScope(); // if stmt
			case 4: break; // IO Stmt.
			case 5: break; // function call statement.
			default: break; // Some other rule number.
		}
		if(currentASTNode->parentNode->sisterNode->ruleNum == 39)
		{
			return;
		}
		else
		{
			currentASTNode = currentASTNode->parentNode->sisterNode->childNode;
		}
	}
	if(currentASTNode == NULL)
		return;
}

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

void createSymbolTables()
{
	fprintf(stderr,"%d and %d\n",currentASTNode->element.nontermValue,currentASTNode->ruleNum);
	int rule = currentASTNode->ruleNum - 7;
	fprintf(stderr,"%d\n",rule);
	switch(rule)
	{
		case 0: findIdentifiers();break; // declaration statement.
		case 1: break; // Assignment stmt type1
		case 2: break; // Assignment stmt type2
		case 3: goToNewScope();break; // if stmt
		case 4: break; // IO Stmt.
		case 5: break; // function call statement.
		default: break; // Some other rule number.
	}
	if(currentASTNode->parentNode->ruleNum == 3)
	{
		fprintf(stderr, "Here we are!!!!\n");
		currentASTNode = currentASTNode->parentNode->sisterNode; // stmtsAndFunctionDef_type1
		fprintf(stderr,"Here we have %d",currentASTNode->element.nontermValue);
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
			currentASTNode = currentASTNode->parentNode->parentNode->sisterNode; //stmtsAndFunctionDefs_type1
			if(currentASTNode == NULL)
				return;
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

void printSymbolTable()
{
	STList readList = headList;
	printf("\n\n");
	printf("%d %s",readList->table->data->symbol,readList->table->data->value);
}

int generateSymbolTables(astTree astRoot)
{
	stList = malloc(sizeof(struct _STList));
	curScope = 0;
	scopeIdentifier = 0;
	initialiseSymbolTable();
	fprintf(stderr,"Heyho!\n");
	currentASTNode = astRoot;
	currentASTNode = currentASTNode->childNode->childNode; // stmtOrFunctionDef rule.
	checkNextStatementAndRead();
	printSymbolTable();
	return 0;
}