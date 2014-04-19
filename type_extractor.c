/* Compilers Project Group 2
// Aditya Raisinghani 2011A7PS042P
// Utkarsh Verma 2011A7PS137P
// BITS Pilani, Pilani Campus
// Second semester 2014
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tokens.h"
#include "lexer.h"
#include "parserDef.h"
#include "first_follow_gen.h"
#include "parser.h"
#include "symbol_table.h"
#include "type_extractor.h"

astTree currentASTNode;
STList stList;
STable currentEntry;
int curScope;
int scopeIdentifier;

void typeError()
{
	fprintf(stderr, "An error was generated\n");
	exit(0);
}

token convertToType(token type)
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

token checkTypeInSymbolTable(char* name)
{
	STable entry = stList->table;
	while(strcmp(entry->data->value,name))
	{
		entry = entry->nextEntry;
		if(entry->data == NULL)
		{
			// All entries in current scope over. Check above scope.
			entry = stList->parentList->table;
		}
	}
	return convertToType(entry->data->type);
}

void checkSizeExpression(token type, astTree nodes)
{
	astTree childID = nodes->childNode->childNode;
	char* idName = childID->data.token_data;
	token typeOfSize = checkTypeInSymbolTable(idName);
	if(typeOfSize != STR)
	{
		typeError();
	}
	else
	{
		if(type != NUM)
			typeError();
	}
}

void checkArithmeticExpression(token type, astTree nodes)
{
	int flag = 0;
	int lowPrecedenceOnly = 0; // For Strings and Matrix
	token acceptedType = type;
	astTree expression = nodes->childNode;
	while(expression != NULL)
	{
		astTree termID = expression->childNode;
		while(termID != NULL)
		{
			astTree factor = termID->childNode;
			if(factor->ruleNum == 54)
			{
				// It is a variable. Check it's type.
				astTree typeNode = factor->childNode;
				if(typeNode->ruleNum == 65)
				{
					// // Check if it is a matrix element.
					if(typeNode->childNode->sisterNode->ruleNum == 77)
					{
						if(acceptedType == RNUM)
						{
							flag = 1;
						}
						else if(acceptedType != NUM)
						{
							typeError();
						}
					}
					else if(typeNode->childNode->sisterNode->ruleNum == 78)
					{
						token idNode = checkTypeInSymbolTable(typeNode->childNode->data.token_data);
						if(acceptedType == RNUM)
						{
							if(idNode == NUM)
							{
								flag = 1;
							}
							else if(idNode != RNUM)
							{
								typeError();
							}
						}
						else if(acceptedType == NUM)
						{
							if(idNode != NUM)
								typeError();
						}
						else if(acceptedType == STR)
						{
							if(idNode != STR)
							{
								typeError();
							}
							else
							{
								lowPrecedenceOnly = 1;
							}
						}
						else if(acceptedType == MATRIX)
						{
							if(idNode != MATRIX)
							{
								typeError();
							}
							else
							{
								lowPrecedenceOnly = 1;
							}
						}
					}
				}
				else if(typeNode->ruleNum == 66)
				{
					// It is a NUM type
					if(acceptedType == RNUM)
						flag = 1;
					else if(acceptedType != NUM)
					{
						typeError();
					}
				}
				else if(typeNode->ruleNum == 67)
				{
					if(acceptedType != RNUM)
						typeError();
				}
				else if(typeNode->ruleNum == 68)
				{
					if(acceptedType != STR)
					{
						typeError();
					}
					else
					{
						lowPrecedenceOnly = 1;
					}
				}
				else
				{
					// Some weird error.
					fprintf(stderr, "You shouldn't have got here.\n");
					exit(0);
				}
			}
			else
			{
				// rule no 53- factor -> (arithmeticExp)
				checkArithmeticExpression(type,factor);
			}
			termID = termID->childNode->sisterNode;
			if(termID->ruleNum == 52)
				termID = NULL;
			else
			{
				if(termID->childNode->ruleNum == 58)
					flag = 0;
				termID = termID->childNode->sisterNode;
			}
			if(lowPrecedenceOnly == 1 || acceptedType == STR || acceptedType == MATRIX)
			{
				if(termID != NULL)
					typeError();
			}
		}
		expression = expression->childNode->sisterNode;
		if(expression->ruleNum == 49)
		{
			expression = NULL;
		}
		else
		{
			expression = expression->childNode->sisterNode;
		}
		if(flag == 1)
			typeError();
	}
}

void checkAssignmentType2()
{
	astTree stmtNode = currentASTNode->childNode->childNode;
	astTree idNodes = stmtNode->childNode;
	astTree rhsNodes = stmtNode->sisterNode;
	if(rhsNodes->ruleNum == 32)
	{
		// It is a size expression for matrix.
		astTree matrixNode = rhsNodes->childNode->childNode;
		token type = checkTypeInSymbolTable(matrixNode->data.token_data);
		if(type != MATRIX)
		{
			// It is something other than matrix. error.
			typeError();
		}
		astTree idNode = idNodes->childNode->childNode;
		token type1 = checkTypeInSymbolTable(idNode->data.token_data);
		idNodes = idNodes->childNode->childNode->sisterNode;
		idNode = idNodes->childNode->childNode;
		token type2 = checkTypeInSymbolTable(idNode->data.token_data);
		idNodes = idNode->sisterNode;
		if(!(type1 == NUM && type2 == NUM && idNodes->ruleNum == 24))
		{
			// Either the types or not correct or there are one too many identifiers.
			typeError();
		}
	}
}

void checkAssignmentType1()
{
	// Current receives current node at stmt.
	astTree stmtNode = currentASTNode->childNode->childNode;
	astTree idNode = stmtNode->childNode;
	char* idName = idNode->data.token_data;
	token type = checkTypeInSymbolTable(idName);
	astTree rhsNodes = stmtNode->sisterNode;
	switch(rhsNodes->ruleNum)
	{
		case 29: checkArithmeticExpression(type,rhsNodes);break;// Arithmetic Expression
		case 30: checkSizeExpression(type,rhsNodes);break;// Size Expression
		case 31: break; // Function call
	}

}

// void goToStatement()
// {
// 	currentASTNode = currentASTNode->childNode;

// }

void popTable()
{
	stList = stList->parentList;
	curScope--;
}

void goToFunctionScope()
{
	curScope++;
	scopeIdentifier++;
	stList = stList->childList;
	while(stList->scopeIdentifier != scopeIdentifier)
	{
		if(stList->sisterList != NULL)
		{
			stList = stList->sisterList;
		}
		else
		{
			exit(0);
		}
	}
	currentASTNode = currentASTNode->childNode->childNode->sisterNode->sisterNode->sisterNode->childNode; // stmtOrFunctionDef
	runTypeChecker();
	popTable();
}

void nextStatement()
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
			popTable();
			currentASTNode = currentASTNode->parentNode->sisterNode; //stmtsAndFunctionDef_type1
			nextStatement();
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

void runTypeChecker()
{
	while(currentASTNode != NULL)
	{
		if(currentASTNode->ruleNum == 4)
		{
			// It is a function.
			goToFunctionScope();
		}
		else
		{
			currentASTNode = currentASTNode->childNode;
			switch(currentASTNode->ruleNum)
			{
				case 7: break;
				case 8: checkAssignmentType1();break;
				case 9: checkAssignmentType2();break;
				case 10: break;
				case 11: break;
				case 12: break; // No return type function
			}
			currentASTNode = currentASTNode->parentNode->sisterNode; //stmtsAndFunctionDefs_type1
			nextStatement();
		}
	}
}

int typeChecker(astTree astRoot, STList headList)
{
	curScope = 0;
	scopeIdentifier = 0;
	currentASTNode = astRoot->childNode->childNode;
	stList = headList;
	runTypeChecker();
	printf("Type checker Ran successfully\n");
	return 0;
}