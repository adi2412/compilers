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
			if(stList->parentList == NULL)
			{
				typeError();
				entry = NULL;
				break;
			}
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

void fillDataInMatrixTable(char* name, int rows, int cols)
{
	matrixSizes entry = stList->matrixTable;
	while(entry->matrixName != NULL)
	{
		entry = entry->nextEntry;
	}
	entry->matrixName = name;
	entry->rows = rows;
	entry->columns = cols;
	entry->nextEntry = malloc(sizeof(struct _matrixSizes));
	entry = entry->nextEntry;
	entry->nextEntry = NULL;
}

void fillDataInStringTable(char* name, int length)
{
	stringSizes entry = stList->stringTable;
	while(entry->stringName != NULL)
	{
		entry = entry->nextEntry;
	}
	entry->stringName = name;
	entry->length = length;
	entry->nextEntry = malloc(sizeof(struct _stringSizes));
	entry = entry->nextEntry;
	entry->nextEntry = NULL;
}

int getSize(char* name)
{
	stringSizes entry = stList->stringTable;
	if(entry->stringName == NULL)
		return 0;
	while(strcmp(entry->stringName,name))
	{
		entry = entry->nextEntry;
		if(entry->stringName == NULL)
		{
			// No such string name found.
			typeError();
			return 0;
		}
	}
	return entry->length;
}

matrixSizes findMatrixInMatrixTable(char* name, int flag)
{
	matrixSizes entry = stList->matrixTable;
	while(strcmp(entry->matrixName,name))
	{
		entry = entry->nextEntry;
		if(entry->matrixName == NULL)
		{
			// All matrices in current scope over. Check above.
			if(stList->parentList == NULL)
			{
				if(flag)
					typeError();
				entry = NULL;
				break;
			}
			entry = stList->parentList->matrixTable;
		}
	}
	return entry;
}

void checkArithmeticExpression(char* idName, token type, astTree nodes)
{
	int flag = 0;
	int lowPrecedenceOnly = 0; // For Strings and Matrix
	token acceptedType = type;
	astTree expression = nodes->childNode;
	int acceptedRows = 0;
	int acceptedCols = 0;
	int stringLength = 0;
	int assignMatrix = 0; // Whether to enter the size in MatrixTable or not.
	if(acceptedType == MATRIX)
	{
		// Find the size of the LHS matrix.
		matrixSizes matrix = findMatrixInMatrixTable(idName,0);
		if(matrix == NULL)
		{
			// The name wasn't found.
			assignMatrix = 1;
		}
		else
		{
			acceptedRows = matrix->rows;
			acceptedCols = matrix->columns;
		}
	}
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
						// If it is an error, ignore this statement and move on after reporting the error.
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
								int length= getSize(typeNode->childNode->data.token_data);
								if(length == 0)
								{
									// No string found error.
									return;
								}
								stringLength += length;
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
								// Check if their sizes match.
								matrixSizes rhsMatrix = findMatrixInMatrixTable(typeNode->childNode->data.token_data,1);
								if(rhsMatrix == NULL)
								{
									// Matrix hasn't been defined.
									return;
								}
								if((acceptedRows != 0 && acceptedCols != 0) && (rhsMatrix->rows != acceptedRows || rhsMatrix->columns != acceptedCols))
								{
									// Sizes of matrix do not match.
									typeError();
									return;
								}
								else if(acceptedRows == 0 && acceptedCols == 0)
								{
									acceptedRows = rhsMatrix->rows;
									acceptedCols = rhsMatrix->columns;
								}
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
				else if(typeNode->ruleNum == 69)
				{
					// Matrix assignment.
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
				checkArithmeticExpression(idName,type,factor);
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
			if(acceptedType == STR && expression->childNode->ruleNum == 56)
				typeError();
			expression = expression->childNode->sisterNode;
		}
		if(flag == 1)
			typeError();
	}
	if(assignMatrix)
	{
		fillDataInMatrixTable(idName,acceptedRows,acceptedCols);
	}
	if(acceptedType == STR && stringLength > 0)
	{
		fillDataInStringTable(idName,stringLength);
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
		case 29: checkArithmeticExpression(idName,type,rhsNodes);break;// Arithmetic Expression
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

void goToElseScope()
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
}

void goToIfScope()
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
	currentASTNode = currentASTNode->childNode->childNode->sisterNode; // stmt in ifStmt rule.
	astTree beforeIfNode = currentASTNode; // to maintain a copy of node we were at.
	runTypeCheckerInIfScope();
	curScope--;
	stList = stList->parentList;
	astTree elseNode = beforeIfNode->sisterNode->sisterNode;
	if(elseNode->ruleNum == 37)
	{
		// No else part.
		currentASTNode = elseNode->parentNode->parentNode; //stmt rule.
		return;
	}
	else
	{
		currentASTNode = elseNode->childNode;
		goToElseScope();
		runTypeCheckerInIfScope();
		curScope--;
		stList = stList->parentList;
		currentASTNode = elseNode->parentNode->parentNode; //stmt rule.
		return;
	}
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

void runTypeCheckerInIfScope()
{
	while(currentASTNode != NULL)
	{
		currentASTNode = currentASTNode->childNode;
		switch(currentASTNode->ruleNum)
		{
			case 7: break;
			case 8: checkAssignmentType1();break;
			case 9: checkAssignmentType2();break;
			case 10: goToIfScope();break; // Need to go into if scope.
			case 11: break;
			case 12: break; // No return type function
		}
		currentASTNode = currentASTNode->parentNode->sisterNode;
		if(currentASTNode->ruleNum == 39)
			currentASTNode = NULL;
		else
		{
			currentASTNode = currentASTNode->childNode;
		}
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
				case 10: goToIfScope();break; // Need to go into if scope.
				case 11: break;
				case 12: break; // No return type function
			}
			currentASTNode = currentASTNode->parentNode->sisterNode; //stmtsAndFunctionDefs_type1
			nextStatement();
		}
	}
}

void printTable(STList headList)
{
	STList readList = headList;
	printf("\n\n");
	printf("Printing all the symbols\n");
	while(readList != NULL)
	{
		printf("%s(%d) From %d to %d\n",readList->functionName,readList->scopeIdentifier,readList->startLineNumber,readList->endLineNumber);
		STable entry = readList->table;
		while(entry->data != NULL)
		{
			printf("%s, scope: %d, type: %d, lineNumber: %d, idType: %d\n",entry->data->value,entry->scope,entry->type,entry->lineNumber,entry->data->type);
			entry = entry->nextEntry;
		}
		printf("matrix sizes\n");
		matrixSizes matrices = readList->matrixTable;
		while(matrices->matrixName != NULL)
		{
			printf("%dx%d, %s\n",matrices->rows,matrices->columns,matrices->matrixName);
			matrices = matrices->nextEntry;
		}
		stringSizes strings = readList->stringTable;
		printf("string sizes\n");
		while(strings->stringName != NULL)
		{
			printf("%d, %s\n",strings->length,strings->stringName);
			strings = strings->nextEntry;
		}
		printf("Going to child table\n");
		if(readList->childList != NULL)
			readList = readList->childList;
		else
			return;
	}
}

int typeChecker(astTree astRoot, STList headList)
{
	curScope = 0;
	scopeIdentifier = 0;
	currentASTNode = astRoot->childNode->childNode;
	stList = headList;
	runTypeChecker();
	printTable(headList);
	printf("Type checker Ran successfully\n");
	return 0;
}