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

void typeError(int error,char* name,int lineNum,int charNum)
{
	switch(error)
	{
		case 0: printf("\x1b[31mSemantic Error: Identifier \x1b[37m\x1b[1m%s\x1b[31m is not a string and cannot be used with size operation on line \x1b[37m\x1b[1m%d:%d\n\x1b[0m",name,lineNum,charNum);break;
		case 1: printf("\x1b[31mSemantic Error: Identifier \x1b[37m\x1b[1m%s\x1b[31m has invalid return type. Not a a number on line \x1b[37m\x1b[1m%d:%d\n\x1b[0m",name,lineNum,charNum);break;
		case 2: printf("\x1b[31mSemantic Error 2: Identifier \x1b[37m\x1b[1m%s\x1b[31m must be declared before use on line \x1b[37m\x1b[1m%d:%d\n\x1b[0m",name,lineNum,charNum);break;
		case 3: printf("\x1b[31mSemantic Error: Identifier \x1b[37m\x1b[1m%s\x1b[31m must be assigned a value before using size operation on line \x1b[37m\x1b[1m%d:%d\n\x1b[0m",name,lineNum,charNum);break;
		case 4: printf("\x1b[31mSemantic Error: Type mismatch error for \x1b[37m\x1b[1m%s\x1b[31m in expression on line \x1b[37m\x1b[1m%d:%d\n\x1b[0m",name,lineNum,charNum);break;
		case 5: printf("\x1b[31mSemantic Error: Size of matrix \x1b[37m\x1b[1m%s\x1b[31m does not match in expression on line \x1b[37m\x1b[1m%d:%d\n\x1b[0m",name,lineNum,charNum);break;
		case 6: printf("\x1b[31mSemantic Error: Only addition and substraction operations allowed for strings and matrices on line number \x1b[37m\x1b[1m%d:%d\n\x1b[0m",lineNum,charNum);break;
		case 7: printf("\x1b[31mSemantic Error: Identifier \x1b[37m\x1b[1m%s\x1b[31m can only have add operation with other strings on line number \x1b[37m\x1b[1m%d:%d\n\x1b[0m",name,lineNum,charNum);break;
		case 8:printf("\x1b[31mSemantic Error: Type mismatch for \x1b[37m\x1b[1m%s\x1b[31m. Number and real number can evaluate only if division operation is used on line number \x1b[37m\x1b[1m%d:%d\n\x1b[0m",name,lineNum,charNum);break;
		case 9:printf("\x1b[31mSemantic Error: Identifier \x1b[37m\x1b[1m%s\x1b[31m is extra. Only two variables returned for size expression on matrix on line \x1b[37m\x1b[1m%d:%d\n\x1b[0m",name,lineNum,charNum);break;
		case 10:printf("\x1b[31mSemantic Error: Only one identifier \x1b[37m\x1b[1m%s\x1b[31m on LHS of size expression on matrix. Requires two on line \x1b[37m\x1b[1m%d:%d\n\x1b[0m",name,lineNum,charNum);break;
	}
	
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

token checkTypeInSymbolTable(char* name,int lineNum, int charNum)
{
	STable entry = stList->table;
	STList readList = stList;
	if(entry->data == NULL)
	{
		if(readList->parentList == NULL)
		{
			typeError(2,name,lineNum,charNum);
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
				typeError(2,name,lineNum,charNum);
				entry = NULL;
				return INVALID;
			}
			entry = readList->parentList->table;
			readList = readList->parentList;
		}
	}
	return convertToType(entry->data->type);
}

/*
// Check the size expression in assignment type stmt
// and report any type errors appropriately.
*/
void checkSizeExpression(token type, astTree nodes)
{
	astTree childID = nodes->childNode->childNode;
	char* idName = childID->data.token_data;
	token typeOfSize = checkTypeInSymbolTable(idName,childID->data.lineNumber,childID->data.charNumber);
	if(typeOfSize != STR)
	{
		typeError(0,idName,childID->data.lineNumber,childID->data.charNumber);
	}
	else
	{
		if(type != NUM)
			typeError(1,idName,childID->data.lineNumber,childID->data.charNumber);
	}
}

/*
// Fills rows and columns entries for a matrix if it does
// not already exist 
*/
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

/**
 * Fills string length for a given string into the symbol
 * table if it does not already have it.
 * @param name
 * @param length
 */
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

/**
 * Gets the size of a particular string from symbol table.
 * @param  name
 * @param  lineNum
 * @param  charNum
 * @param  list
 * @param  flag
 * @return
 */
int getSize(char* name,int lineNum,int charNum,STList list,int flag)
{
	STList readList = list;
	if(list->stringTable->stringName != NULL)
	{
		stringSizes strings = readList->stringTable;
		while(strings->stringName != NULL)
		{
			if(strcmp(strings->stringName,name))
				strings = strings->nextEntry;
			else
				return strings->length;
			if(strings->stringName == NULL)
			{
				int length = 0;
				if(readList->parentList != NULL)
				{
					readList = readList->parentList;
					length = getSize(name,lineNum,charNum,readList,1);
				}
				if(length == 0 && flag)
				{
					if(readList->sisterList != NULL)
					{
						length = getSize(name,lineNum,charNum,readList->sisterList,1);
					}
				}
				if(length == 0 && !flag)
				{
					typeError(3,name,lineNum,charNum);
				}
				return length;
			}
		}
	}
	typeError(3,name,lineNum,charNum);
	return 0;
}

/**
 * Gets the rows and columns size of a matrix from symbol
 * table.
 * @param  name
 * @param  flag
 * @param  lineNum
 * @param  charNum
 * @return
 */
matrixSizes findMatrixInMatrixTable(char* name, int flag, int lineNum, int charNum)
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
					typeError(3,name,lineNum,charNum);
				entry = NULL;
				break;
			}
			entry = stList->parentList->matrixTable;
		}
	}
	return entry;
}

/**
 * Checks an arithmetic expression and reports type errors
 * if any.
 * @param idName
 * @param type
 * @param nodes
 */
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
		astTree temp = expression->childNode->childNode->childNode->childNode;
		matrixSizes matrix = findMatrixInMatrixTable(idName,0,temp->data.lineNumber,temp->data.charNumber);
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
			if(factor->ruleNum == 48)
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
							typeError(4,typeNode->childNode->data.token_data,typeNode->childNode->data.lineNumber,typeNode->childNode->data.charNumber);
						}
					}
					else if(typeNode->childNode->sisterNode->ruleNum == 78)
					{
						token idNode = checkTypeInSymbolTable(typeNode->childNode->data.token_data,typeNode->childNode->data.lineNumber,typeNode->childNode->data.charNumber);
						if(idNode == INVALID)
							return;
						// If it is an error, ignore this statement and move on after reporting the error.
						if(acceptedType == RNUM)
						{
							if(idNode == NUM)
							{
								flag = 1;
							}
							else if(idNode != RNUM)
							{
								typeError(4,typeNode->childNode->data.token_data,typeNode->childNode->data.lineNumber,typeNode->childNode->data.charNumber);
							}
						}
						else if(acceptedType == NUM)
						{
							if(idNode != NUM)
								typeError(4,typeNode->childNode->data.token_data,typeNode->childNode->data.lineNumber,typeNode->childNode->data.charNumber);
						}
						else if(acceptedType == STR)
						{
							if(idNode != STR)
							{
								typeError(4,typeNode->childNode->data.token_data,typeNode->childNode->data.lineNumber,typeNode->childNode->data.charNumber);
							}
							else
							{
								int length= getSize(typeNode->childNode->data.token_data,typeNode->childNode->data.lineNumber,typeNode->childNode->data.charNumber,stList,0);
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
								typeError(4,typeNode->childNode->data.token_data,typeNode->childNode->data.lineNumber,typeNode->childNode->data.charNumber);
							}
							else
							{
								// Check if their sizes match.
								matrixSizes rhsMatrix = findMatrixInMatrixTable(typeNode->childNode->data.token_data,1,typeNode->childNode->data.lineNumber,typeNode->childNode->data.charNumber);
								if(rhsMatrix == NULL)
								{
									// Matrix hasn't been defined.
									return;
								}
								if((acceptedRows != 0 && acceptedCols != 0) && (rhsMatrix->rows != acceptedRows || rhsMatrix->columns != acceptedCols))
								{
									// Sizes of matrix do not match.
									typeError(5,typeNode->childNode->data.token_data,typeNode->childNode->data.lineNumber,typeNode->childNode->data.charNumber);
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
						typeError(4,typeNode->childNode->data.token_data,typeNode->childNode->data.lineNumber,typeNode->childNode->data.charNumber);
					}
				}
				else if(typeNode->ruleNum == 67)
				{
					if(acceptedType != RNUM)
						typeError(4,typeNode->childNode->data.token_data,typeNode->childNode->data.lineNumber,typeNode->childNode->data.charNumber);
				}
				else if(typeNode->ruleNum == 68)
				{
					if(acceptedType != STR)
					{
						typeError(4,typeNode->childNode->data.token_data,typeNode->childNode->data.lineNumber,typeNode->childNode->data.charNumber);
					}
					else
					{
						int length = strlen(typeNode->childNode->data.token_data);
						stringLength += length;
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
			if(termID->ruleNum == 54)
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
					typeError(6,termID->childNode->data.token_data,termID->childNode->childNode->data.lineNumber,termID->childNode->childNode->data.charNumber);
			}
		}
		expression = expression->childNode->sisterNode;
		if(expression->ruleNum == 51)
		{
			expression = NULL;
		}
		else
		{
			if(acceptedType == STR && expression->childNode->ruleNum == 56)
				typeError(7,expression->childNode->data.token_data,expression->childNode->childNode->data.lineNumber,expression->childNode->childNode->data.charNumber);
			expression = expression->childNode->sisterNode;
		}
		if(flag == 1)
			typeError(8,expression->childNode->data.token_data,expression->childNode->childNode->data.lineNumber,expression->childNode->childNode->data.charNumber);
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

/**
 * Checks assignmenttype2 statements for type errors.
 */
void checkAssignmentType2()
{
	astTree stmtNode = currentASTNode->childNode->childNode;
	astTree idNodes = stmtNode;
	astTree rhsNodes = stmtNode->sisterNode;
	if(rhsNodes->ruleNum == 29)
	{
		// It is a size expression for matrix.
		astTree matrixNode = rhsNodes->childNode->childNode;
		token type = checkTypeInSymbolTable(matrixNode->data.token_data,matrixNode->data.lineNumber,matrixNode->data.charNumber);
		if(type == INVALID)
			return;
		if(type != MATRIX)
		{
			// It is something other than matrix. error.
			typeError(4,matrixNode->data.token_data,matrixNode->data.lineNumber,matrixNode->data.charNumber);
		}
		astTree idNode = idNodes->childNode->childNode;
		token type1 = checkTypeInSymbolTable(idNode->data.token_data,idNode->data.lineNumber,idNode->data.charNumber);
		if(type1 == INVALID)
			return;
		idNodes = idNodes->childNode->childNode->sisterNode;
		if(idNodes->ruleNum == 33)
		{
			typeError(10,idNode->data.token_data,idNode->data.lineNumber,idNode->data.charNumber);
		}
		idNode = idNodes->childNode->childNode;
		token type2 = checkTypeInSymbolTable(idNode->data.token_data,idNode->data.lineNumber,idNode->data.charNumber);
		if(type2 == INVALID)
			return;
		idNodes = idNode->sisterNode;
		if(!(type1 == NUM && type2 == NUM && idNodes->ruleNum == 33))
		{
			// Either the types or not correct or there are one too many identifiers.
			typeError(9,idNodes->childNode->data.token_data,idNodes->childNode->data.lineNumber,idNodes->childNode->data.charNumber);
		}
	}
}

/**
 * Checks assignment type1 statements for type errors.
 */
void checkAssignmentType1()
{
	// Current receives current node at stmt.
	astTree stmtNode = currentASTNode->childNode->childNode;
	astTree idNode = stmtNode->childNode;
	char* idName = idNode->data.token_data;
	token type = checkTypeInSymbolTable(idName,idNode->data.lineNumber,idNode->data.charNumber);
	if(type == INVALID)
		return;
	astTree rhsNodes = stmtNode->sisterNode;
	switch(rhsNodes->ruleNum)
	{
		case 26: checkArithmeticExpression(idName,type,rhsNodes);break;// Arithmetic Expression
		case 27: checkSizeExpression(type,rhsNodes);break;// Size Expression
		case 28: break; // Function call
	}
}

/**
 * Pops the symbol table from the current symbol table.
 */
void popTable()
{
	stList = stList->parentList;
	curScope--;
}

/**
 * Goes to else scope and pushes its symbol table to the
 * current symbol table.
 */
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

/**
 * Goes to if scope and pushes its symbol table to the 
 * current symbol table.
 */
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

/**
 * Goes to new function scope and pushes its symbol table
 * to the current symbol table.
 */
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

/**
 * Get the next statement to run type checker on.
 */
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

/**
 * Separate function to run type checker when in if scope.
 */
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

/**
 * Run type checker when in a function scope.
 */
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

/**
 * Find length of a string from the symbol table. To be
 * used only with print symbol table function.
 * @param  name
 * @param  list
 * @param  flag
 * @return
 */
int findLength(char* name, STList list, int flag)
{
	STList readList = list;
	if(list->stringTable->stringName != NULL)
	{
		stringSizes strings = readList->stringTable;
		while(strings->stringName != NULL)
		{
			if(strcmp(strings->stringName,name))
				strings = strings->nextEntry;
			else
				return strings->length;
			if(strings->stringName == NULL)
			{
				int length = 0;
				if(readList->parentList != NULL)
				{
					readList = readList->parentList;
					length = findLength(name,readList,1);
				}
				if(length == 0 && flag)
				{
					if(readList->sisterList != NULL)
					{
						length = findLength(name,readList->sisterList,1);
					}
				}
				return length;
			}
		}
	}
	return 0;
}

/**
 * Find the rows and columns of a mtrix from the symbol
 * table. To be used only with print symbol table function.
 * @param  name
 * @param  list
 * @return
 */
matrixSizes findMatrix(char* name,STList list)
{
	STList readList = list;
	if(list->matrixTable != NULL)
	{
		matrixSizes entry = list->matrixTable;
		if(entry->matrixName != NULL)
		{
			while(strcmp(entry->matrixName,name))
			{
				entry = entry->nextEntry;
				if(entry->matrixName == NULL)
				{
					// All matrices in current scope over. Check above.
					if(readList->parentList == NULL)
					{
						entry = NULL;
						break;
					}
					entry = readList->parentList->matrixTable;
					readList = readList->parentList;
				}
			}
		}
		else
			entry = NULL;
		return entry;
	}
	return NULL;
}

/**
 * Print the symbol table in the specified format.
 * @param list
 */
void printSymbolTable(STList list)
{
	int a = 0;
	STList readList = list;
	while(readList != NULL)
	{
		STable entry = readList->table;
		while(entry->data != NULL)
		{
			printf("%20s%16s(%2d-%2d)%16s",entry->data->value,readList->functionName,readList->startLineNumber,readList->endLineNumber,getTokenName(entry->data->type));
			if(entry->data->type == NUM || entry->data->type == INT)
			{
				printf("%s","    ");
				printf("%15d",a);
				a = a+2;
			}
			else if(entry->data->type == RNUM || entry->data->type == REAL)
			{
				printf("%s","    ");
				printf("%15d",a);
				a= a+4;
			}
			else if(entry->data->type == STR || entry->data->type == STRING)
			{
				printf("%s","    ");
				printf("%15d",a);
				a = a + findLength(entry->data->value,readList,0);
			}
			else if(entry->data->type == MATRIX)
			{
				matrixSizes mat = findMatrix(entry->data->value,readList);
				if(mat == NULL)
				{
					printf(",0,0");
				}
				else
				{
					printf(",%d,%d",mat->rows,mat->columns);
				}
			}
			printf("\n");
			entry = entry->nextEntry;
		}
		if(readList->childList != NULL)
			printSymbolTable(readList->childList);
		if(readList->sisterList != NULL)
			readList = readList->sisterList;
		else
			readList = NULL;
	}
}

/**
 * Main function called by driver to run the type checker
 * on the entire code using AST as input.
 * @param  astRoot
 * @param  headList
 * @return
 */
int typeChecker(astTree astRoot, STList headList)
{
	curScope = 0;
	scopeIdentifier = 0;
	currentASTNode = astRoot->childNode->childNode;
	stList = headList;
	runTypeChecker();
	return 0;
}