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
#include "semantic_analyzer.h"

astTree currentASTNode;
STList stList;
STable currentEntry;
int curScope;
int scopeIdentifier;
int returnParamsAssignedValue;

void semanticError()
{
	fprintf(stderr, "A semantic error was generated\n");
	exit(0);
}

void popTableInSemanticAnalyzer()
{
	stList = stList->parentList;
	curScope--;
	if(returnParamsAssignedValue != 0)
	{
		// All return variables have not been assigned a value.
		semanticError();
		return;
	}
}

// int doesExistInSymbolTable(char* name)
// {
// 	STable entry = stList->table;
// 	while(strcmp(entry->data->value, name))
// 	{
// 		entry = entry->nextEntry;
// 		if(entry->data == NULL)
// 		{
// 			// All entries in the current scope over. Check parent scope.
// 			if(stList->parentList == NULL)
// 				return 0;
// 			entry = stList->parentList->table;
// 		}
// 	}
// 	return 1;
// }

STList findFunctionSymbolTable(char* name, int startLineNumber, int endLineNumber)
{
	STList findList = stList->childList;
	STable entry = findList->table;
	while(strcmp(findList->functionName,name))
	{
		if(findList->sisterList == NULL)
		{
			findList = NULL;
			break;
		}
		findList = findList->sisterList;
	}
	// Now check in the parent's scope for the function.
	if(findList == NULL)
	{
		findList = stList->parentList->childList;
		while(strcmp(findList->functionName,name))
		{
			if(findList->sisterList == NULL)
			{
				if(findList->parentList->parentList == NULL)
				{
					// All scopes checked and function name not found.
					semanticError();
				}
				else
				{
					findList = findList->parentList->parentList->childList;
				}
			}
			else
			{
				findList = findList->sisterList;
			}
		}
	}
	if(findList != NULL)
	{
		if((findList->startLineNumber < startLineNumber && findList->endLineNumber < startLineNumber) || (findList->startLineNumber > startLineNumber && findList->endLineNumber < endLineNumber))
		{
			// Function being called should have been defined before in parent scope or within the current scope. If in parent's scope, the function definition should have ended before the start of this function as well. If it is in current scope, it should be completely within it. Checking that the end line number is also before the call statement ensures no recursive calls are made.
			return findList;
		}
		else
		{
			// Do separate semantic errors for recursive calls and for function call before definition error.
			semanticError();
		}
	}
	return findList;
}

// void // {
// 	astTree declarationStmt = currentASTNode->childNode;
// 	astTree idNodes = declarationStmt->childNode->sisterNode;
// 	while(idNodes != NULL)
// 	{
// 		astTree idNode = idNodes->childNode;
// 		if(!doesExistInSymbolTable(idNode->data.token_data))
// 		{
// 			// Same name variable exists in current scope.
// 			semanticError();
// 		}
// 		else
// 		{
// 			idNodes = idNodes->childNode->sisterNode;
// 			if(idNodes->ruleNum == 24)
// 				idNodes = NULL;
// 			else
// 				idNodes = idNodes->childNode;
// 		}
// 	}
// }

void analyzeAssignmentStmt()
{
	// Only need to check if it is a fun call stmt and analyze that.
	printf("analyzing assignment statement\n");
	astTree assignmentStmt = currentASTNode->childNode;
	astTree rhsNodes = assignmentStmt->childNode->sisterNode;
	if(rhsNodes->ruleNum == 31)
	{
		astTree returnID = assignmentStmt->childNode->childNode;
		token returnType = checkTypeInSymbolTable(returnID->data.token_data);
		STList funcSymbolTable = findFunctionSymbolTable(rhsNodes->childNode->childNode->data.token_data,stList->startLineNumber,stList->endLineNumber);
		if(funcSymbolTable == NULL)
		{
			// Semantic error. Leave this statement and go to next statement.
			return;
		}
		token returnParamType;
		if(funcSymbolTable->table->type == RETURN)
			returnParamType = convertToType(funcSymbolTable->table->data->type);
		else
		{
			// This function has no return values.
			semanticError();
		}
		if(returnParamType != returnType)
		{
			// Return type does not match variable on LHS.
			printf("%d, %d\n",returnParamType, returnType);
			semanticError();
		}
		else
		{
			if(funcSymbolTable->table->nextEntry->type == RETURN)
			{
				// More return values, but only one variable on LHS.
				semanticError();
			}
		}

		// Check that the parameters being passed are the same as the one required by the function.
		astTree inputParaList = rhsNodes->childNode->childNode->sisterNode;
		STable inputParams = funcSymbolTable->table->nextEntry;
		while(inputParams->type == PARA)
		{
			if(inputParaList == NULL)
			{
				// No input parameters available, and function requires more.
				semanticError();
				return;
			}
			else
			{
				astTree inputParam = inputParaList->childNode;
				token inputParamType;
				if(inputParam->ruleNum == 65)
				{
					// It is an ID.
					if(inputParam->childNode->sisterNode->ruleNum == 77)
					{
						// It is a matrix element.
						inputParamType = NUM;
					}
					else
					{
						inputParamType = checkTypeInSymbolTable(inputParam->childNode->data.token_data);
						if(inputParamType == NIL)
						{
							// There was an error. Move on to next statement.
							return;
						}
					}
				}
				else if(inputParam->ruleNum == 66)
				{
					inputParamType = NUM;
				}
				else if(inputParam->ruleNum == 67)
				{
					inputParamType = RNUM;
				}
				else if(inputParam->ruleNum == 68)
				{
					inputParamType = STR;
				}
				else
				{
					inputParamType = MATRIX;
				}
				if(convertToType(inputParams->data->type) != inputParamType)
				{
					// Type mismatch. Move on to next statement.
					semanticError();
					return;
				}
			}
			inputParams = inputParams->nextEntry;
			inputParaList = inputParaList->childNode->sisterNode;
			if(inputParaList->ruleNum == 46)
			{
				inputParaList = NULL;
			}
			else
			{
				inputParaList = inputParaList->childNode;
			}
		}

		// All function parameters are over.
		// Check if the input parameter list is also over.
		if(inputParaList != NULL)
		{
			if(inputParaList->ruleNum != 46)
			{
				// There are extra input parameters being sent.
				semanticError();
				return;
			}
		}
	}
	else
	{
		// Check if left hand side is one of the return values.
		astTree LHSNode = assignmentStmt->childNode;
		astTree idNode = LHSNode->childNode;
		STable entry = stList->table;
		while(entry->type == RETURN)
		{
			if(!strcmp(entry->data->value,idNode->data.token_data))
				returnParamsAssignedValue--;
			entry = entry->nextEntry;
		}
	}
}

void analyzeFunCallStmt()
{
	// Check that this function call statement has no return values.
	astTree funCallStmt = currentASTNode->childNode;	
	STList funcSymbolTable = findFunctionSymbolTable(funCallStmt->childNode->data.token_data,stList->startLineNumber,stList->endLineNumber);
	if(funcSymbolTable == NULL)
	{
		// Semantic error. Leave this statement and go to next statement.
		return;
	}
	if(funcSymbolTable->table->type == RETURN)
	{
		// This function has one or more return values.
		semanticError();
		return;
	}


	// Check input parameters.
	astTree inputParaList = funCallStmt->childNode->sisterNode;
	STable inputParams = funcSymbolTable->table->nextEntry;
	while(inputParams->type == PARA)
	{
		if(inputParaList == NULL)
		{
			// No input parameters available, and function requires more.
			semanticError();
			return;
		}
		else
		{
			astTree inputParam = inputParaList->childNode;
			token inputParamType;
			if(inputParam->ruleNum == 65)
			{
				// It is an ID.
				if(inputParam->childNode->sisterNode->ruleNum == 77)
				{
					// It is a matrix element.
					inputParamType = NUM;
				}
				else
				{
					inputParamType = checkTypeInSymbolTable(inputParam->childNode->data.token_data);
					if(inputParamType == NIL)
					{
						// There was an error. Move on to next statement.
						return;
					}
				}
			}
			else if(inputParam->ruleNum == 66)
			{
				inputParamType = NUM;
			}
			else if(inputParam->ruleNum == 67)
			{
				inputParamType = RNUM;
			}
			else if(inputParam->ruleNum == 68)
			{
				inputParamType = STR;
			}
			else
			{
				inputParamType = MATRIX;
			}
			if(convertToType(inputParams->data->type) != inputParamType)
			{
				// Type mismatch. Move on to next statement.
				semanticError();
				return;
			}
		}
		inputParams = inputParams->nextEntry;
		inputParaList = inputParaList->childNode->sisterNode;
		if(inputParaList->ruleNum == 46)
		{
			inputParaList = NULL;
		}
		else
		{
			inputParaList = inputParaList->childNode;
		}
	}

	// All function parameters are over.
	// Check if the input parameter list is also over.
	if(inputParaList->ruleNum != 46)
	{
		// There are extra input parameters being sent.
		semanticError();
		return;
	}
}

void analyzeAssignmentType2Stmt()
{
	// Only need to check if it is a fun call stmt and analyze that.
	astTree assignmentStmt = currentASTNode->childNode;
	astTree rhsNodes = assignmentStmt->childNode->sisterNode;
	if(rhsNodes->ruleNum == 33)
	{
		astTree returnList = assignmentStmt->childNode->childNode;
		STList funcSymbolTable = findFunctionSymbolTable(rhsNodes->childNode->childNode->data.token_data,stList->startLineNumber,stList->endLineNumber);
		if(funcSymbolTable == NULL)
		{
			// Semantic error. Leave this statement and go to next statement.
			return;
		}
		STable returnParams = funcSymbolTable->table;
		while(returnParams->type == RETURN)
		{
			if(returnList == NULL)
			{
				// Function returns more variables than provided.
				semanticError();
				return;
			}
			else
			{
				astTree returnParam = returnList->childNode;
				token returnParamType;
				returnParamType = checkTypeInSymbolTable(returnParam->childNode->data.token_data);
				if(returnParamType == NIL)
				{
					// There was an error. Move on to the next statement.
					return;
				}
				if(returnParamType != convertToType(returnParams->data->type))
				{
					// Type mismatch. Move on to next statement.
					semanticError();
					return;
				}
			}
			returnParams = returnParams->nextEntry;
			returnList = returnList->childNode->sisterNode;
			if(returnList->ruleNum == 24)
				returnList = NULL;
			else
				returnList = returnList->childNode;
		}

		// Check that the parameters being passed are the same as the one required by the function.
		astTree inputParaList = rhsNodes->childNode->childNode->sisterNode;
		STable inputParams = funcSymbolTable->table->nextEntry;
		while(inputParams->type == PARA)
		{
			if(inputParaList == NULL)
			{
				// No input parameters available, and function requires more.
				semanticError();
				return;
			}
			else
			{
				astTree inputParam = inputParaList->childNode;
				token inputParamType;
				if(inputParam->ruleNum == 65)
				{
					// It is an ID.
					if(inputParam->childNode->sisterNode->ruleNum == 77)
					{
						// It is a matrix element.
						inputParamType = NUM;
					}
					else
					{
						inputParamType = checkTypeInSymbolTable(inputParam->childNode->data.token_data);
						if(inputParamType == NIL)
						{
							// There was an error. Move on to next statement.
							return;
						}
					}
				}
				else if(inputParam->ruleNum == 66)
				{
					inputParamType = NUM;
				}
				else if(inputParam->ruleNum == 67)
				{
					inputParamType = RNUM;
				}
				else if(inputParam->ruleNum == 68)
				{
					inputParamType = STR;
				}
				else
				{
					inputParamType = MATRIX;
				}
				if(convertToType(inputParams->data->type) != inputParamType)
				{
					// Type mismatch. Move on to next statement.
					semanticError();
					return;
				}
			}
			inputParams = inputParams->nextEntry;
			inputParaList = inputParaList->childNode->sisterNode;
			if(inputParaList->ruleNum == 46)
			{
				inputParaList = NULL;
			}
			else
			{
				inputParaList = inputParaList->childNode;
			}
		}

		// All function parameters are over.
		// Check if the input parameter list is also over.
		if(inputParaList->ruleNum != 46)
		{
			// There are extra input parameters being sent.
			semanticError();
			return;
		}
	}
	else
	{
		// Check if any of the LHS nodes is a return variable.
		astTree LHSNodes = assignmentStmt->childNode->childNode;
		while(LHSNodes != NULL)
		{
			astTree idNode = LHSNodes->childNode;
			STable entry = stList->table;
			while(entry->type == RETURN)
			{
				if(!strcmp(entry->data->value,idNode->data.token_data))
					returnParamsAssignedValue--;
				entry = entry->nextEntry;
			}
			LHSNodes = LHSNodes->childNode->sisterNode;
			if(LHSNodes->ruleNum == 24)
				LHSNodes = NULL;
			else
			{
				LHSNodes = LHSNodes->childNode;
			}
		}
	}
}

void analyzeFunctionScope()
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
	STable returnEntries = stList->table;
	while(returnEntries->type == RETURN)
	{
		returnParamsAssignedValue++;
		returnEntries = returnEntries->nextEntry;
	}
	runSemanticAnalyzer();
	popTableInSemanticAnalyzer();
}

void ToElseScope()
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

void analyzeIfStmt()
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
	runSemanticAnalyzerInIfScope();
	curScope--;
	stList = stList->parentList;
	astTree elseNode = beforeIfNode->sisterNode->sisterNode;
	if(elseNode->ruleNum == 37)
	{
		// No else part.
		currentASTNode = elseNode->parentNode->parentNode;
		return;
	}
	else
	{
		currentASTNode = elseNode->childNode;
		ToElseScope();
		runSemanticAnalyzerInIfScope();
		curScope--;
		stList = stList->parentList;
		currentASTNode = elseNode->parentNode->parentNode; //stmt rule.
		return;
	}
}

void toNextStatement()
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
			popTableInSemanticAnalyzer();
			currentASTNode = currentASTNode->parentNode->sisterNode; // stmtsAndFunctionDef_type1
			toNextStatement();
			return;
		}
	}
	else
	{
		// More statements exist.
		currentASTNode = currentASTNode->childNode->childNode; // stmtOrFunctionDef
		return;
	}
}

void runSemanticAnalyzerInIfScope()
{
	while(currentASTNode != NULL)
	{
		currentASTNode = currentASTNode->childNode;
		switch(currentASTNode->ruleNum)
		{
			case 7: break;
			case 8: analyzeAssignmentStmt();break;
			case 9: analyzeAssignmentType2Stmt();break;
			case 10: analyzeIfStmt();break;
			case 11: break;
			case 12: analyzeFunCallStmt();break;
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

void runSemanticAnalyzer()
{
	printf("going through a statement\n");
	while(currentASTNode != NULL)
	{
		printf("statement!\n");
		if(currentASTNode->ruleNum == 4)
		{
			// It is a function.
			// Also check if a function with same name exists in scope.
			analyzeFunctionScope();
		}
		else
		{
			currentASTNode = currentASTNode->childNode;
			switch(currentASTNode->ruleNum)
			{
				case 7: break;
				case 8: analyzeAssignmentStmt();break;
				case 9: analyzeAssignmentType2Stmt();break;
				case 10: analyzeIfStmt();break;
				case 11: break;
				case 12: analyzeFunCallStmt();break;
			}
			currentASTNode = currentASTNode->parentNode->sisterNode;
			toNextStatement();
		}
	}
}

int semanticAnalyzer(astTree astRoot, STList headList)
{
	curScope = 0;
	scopeIdentifier = 0;
	returnParamsAssignedValue = 0;
	currentASTNode = astRoot->childNode->childNode;
	stList = headList;
	runSemanticAnalyzer();
	printf("Semantic analyzer ran successfully\n");
	return 0;
}