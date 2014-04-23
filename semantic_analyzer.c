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
#include "sem_parser.h"
#include "ast.h"
#include "symbol_table.h"
#include "type_extractor.h"
#include "semantic_analyzer.h"

astTree currentASTNode;
STList stList;
STable currentEntry;
int curScope;
int scopeIdentifier;
int returnParamsAssignedValue;

void semanticError(int num,char* name, int lineNum, int charNum)
{
	switch(num)
	{
		case 0: printf("\x1b[31mSemantic Error: All return variables for function \x1b[37m\x1b[1m%s\x1b[31m have not been assigned a value.\n\x1b[0m",name);break;
		case 1: printf("\x1b[31mSemantic Error: Function %s has been called but not defined on line \x1b[37m\x1b[1m%d:%d\n\x1b[0m",name,lineNum,charNum);break;
		case 2: printf("\x1b[31mSemantic Error: Function \x1b[37m\x1b[1m%s\x1b[31m has been called recursively on line \x1b[37m\x1b[1m%d:%d\n\x1b[0m",name,lineNum,charNum);break;
		case 3: printf("\x1b[31mSemantic Error: Variable \x1b[37m\x1b[1m%s\x1b[31m being assigned to a function with no return value on line \x1b[37m\x1b[1m%d:%d\n\x1b[0m",name,lineNum,charNum);break;
		case 4: printf("\x1b[31mSemantic Error: Type mismatch- variable \x1b[37m\x1b[1m%s\x1b[31m has different type than return value on line \x1b[37m\x1b[1m%d:%d\n\x1b[0m",name,lineNum,charNum);break;
		case 5: printf("\x1b[31mSemantic Error: Function \x1b[37m\x1b[1m%s\x1b[31m has been more than one return type but only one being assigned on line \x1b[37m\x1b[1m%d:%d\n\x1b[0m",name,lineNum,charNum);break;
		case 6: printf("\x1b[31mSemantic Error: Too few input/return parameters to function \x1b[37m\x1b[1m%s\x1b[31m on line \x1b[37m\x1b[1m%d:%d\n\x1b[0m",name,lineNum,charNum);break;
		case 7: printf("\x1b[31mSemantic Error: Type mismatch for function parameter name \x1b[37m\x1b[1m%s\x1b[31m on line \x1b[37m\x1b[1m%d:%d\n\x1b[0m",name,lineNum,charNum);break;
		case 8: printf("\x1b[31mSemantic Error: Too many input parameters being sent on line \x1b[37m\x1b[1m%d:%d\n\x1b[0m",lineNum,charNum);break;
		case 9: printf("\x1b[31mSemantic Error: Non void function \x1b[37m\x1b[1m%s\x1b[31m being called as a void on line \x1b[37m\x1b[1m%d:%d\n\x1b[0m",name,lineNum,charNum);break;
	}
}

/**
 * Pop the symbol table from the current symbol table.
 */
void popTableInSemanticAnalyzer()
{
	if(returnParamsAssignedValue != 0)
	{
		// All return variables have not been assigned a value.
		semanticError(0,stList->functionName,0,0);
		returnParamsAssignedValue = 0;
	}
	stList = stList->parentList;
	curScope--;
}

/**
 * Find the symbol table for a particular function name.
 * Pass the current function's start and end line number
 * to check if it is a legal call and if it is not a
 * recursive call.
 * @param  name
 * @param  startLineNumber
 * @param  endLineNumber
 * @param  lineNum
 * @param  colNum
 * @return
 */
STList findFunctionSymbolTable(char* name, int startLineNumber, int endLineNumber, int lineNum, int colNum)
{
	if(!strcmp(name,stList->functionName))
	{
		semanticError(2,name,lineNum,colNum);
		return NULL;
	}
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
					semanticError(1,name,lineNum,colNum);
					return NULL;
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
			semanticError(2,name,lineNum,colNum);
			return NULL;
		}
	}
	return findList;
}

token findTypeInSymbolTable(STList list,char* name,int lineNum, int charNum)
{
	STable entry = list->table;
	STList readList = list;
	if(entry->data == NULL)
	{
		if(readList->parentList == NULL)
		{
			// typeError(2,name,lineNum,charNum);
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
				// typeError(2,name,lineNum,charNum);
				entry = NULL;
				return INVALID;
			}
			entry = readList->parentList->table;
			readList = readList->parentList;
		}
	}
	return convertToType(entry->data->type);
}

/**
 * Analyze the current assignment statement for semantic
 * errors.
 */
void analyzeAssignmentStmt()
{
	// Only need to check if it is a fun call stmt and analyze that.
	astTree assignmentStmt = currentASTNode->childNode;
	astTree rhsNodes = assignmentStmt->childNode->sisterNode;
	if(rhsNodes->ruleNum == 28)
	{
		astTree returnID = assignmentStmt->childNode->childNode;
		token returnType = checkTypeInSymbolTable(returnID->data.token_data,returnID->data.lineNumber,returnID->data.charNumber);
		STList funcSymbolTable = findFunctionSymbolTable(rhsNodes->childNode->childNode->data.token_data,stList->startLineNumber,stList->endLineNumber,returnID->data.lineNumber,returnID->data.charNumber);
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
			semanticError(3,returnID->data.token_data,returnID->data.lineNumber,returnID->data.charNumber);
			return;
		}
		if(returnParamType != returnType)
		{
			// Return type does not match variable on LHS.
			semanticError(4,returnID->data.token_data,returnID->data.lineNumber,returnID->data.charNumber);
			return;
		}
		else
		{
			if(funcSymbolTable->table->nextEntry->type == RETURN)
			{
				// More return values, but only one variable on LHS.
				semanticError(5,funcSymbolTable->functionName,returnID->data.lineNumber,returnID->data.charNumber);
				return;
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
				semanticError(6,funcSymbolTable->functionName,returnID->data.lineNumber,returnID->data.charNumber);
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
						inputParamType = checkTypeInSymbolTable(inputParam->childNode->data.token_data,inputParam->childNode->data.lineNumber,inputParam->childNode->data.charNumber);
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
					semanticError(7,inputParams->data->value,inputParam->childNode->data.lineNumber,inputParam->childNode->data.charNumber);
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
				if(inputParaList->childNode->ruleNum != 44)
				{
					semanticError(8,"",inputParaList->childNode->childNode->data.lineNumber,inputParaList->childNode->childNode->data.lineNumber);
					return;
				}
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

/**
 * Analyze the function call statement for semantic errors.
 */
void analyzeFunCallStmt()
{
	// Check that this function call statement has no return values.
	astTree funCallStmt = currentASTNode->childNode;	
	STList funcSymbolTable = findFunctionSymbolTable(funCallStmt->childNode->data.token_data,stList->startLineNumber,stList->endLineNumber,funCallStmt->childNode->data.lineNumber,funCallStmt->childNode->data.charNumber);
	if(funcSymbolTable == NULL)
	{
		// Semantic error. Leave this statement and go to next statement.
		return;
	}
	if(funcSymbolTable->table->type == RETURN)
	{
		// This function has one or more return values.
		semanticError(9,funcSymbolTable->functionName,funCallStmt->childNode->data.lineNumber,funCallStmt->childNode->data.charNumber);
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
			semanticError(6,funcSymbolTable->functionName,funCallStmt->childNode->data.lineNumber,funCallStmt->childNode->data.charNumber);
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
					inputParamType = checkTypeInSymbolTable(inputParam->childNode->data.token_data,inputParam->childNode->data.lineNumber,inputParam->childNode->data.charNumber);
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
				semanticError(7,inputParams->data->value,inputParam->childNode->data.lineNumber,inputParam->childNode->data.charNumber);
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
		semanticError(8,"",inputParaList->childNode->childNode->childNode->data.lineNumber,inputParaList->childNode->childNode->childNode->data.lineNumber);
		return;
	}
}

/**
 * Analyze assignment type2 statement for semantic errors.
 */
void analyzeAssignmentType2Stmt()
{
	// Only need to check if it is a fun call stmt and analyze that.
	astTree assignmentStmt = currentASTNode->childNode;
	astTree rhsNodes = assignmentStmt->childNode->sisterNode;
	if(rhsNodes->ruleNum == 30)
	{
		astTree returnList = assignmentStmt->childNode->childNode;
		STList funcSymbolTable = findFunctionSymbolTable(rhsNodes->childNode->childNode->data.token_data,stList->startLineNumber,stList->endLineNumber,rhsNodes->childNode->childNode->data.lineNumber,rhsNodes->childNode->childNode->data.charNumber);
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
				semanticError(6,funcSymbolTable->functionName,rhsNodes->childNode->childNode->data.lineNumber,rhsNodes->childNode->childNode->data.charNumber);
				return;
			}
			else
			{
				astTree returnParam = returnList->childNode;
				token returnParamType;
				if(returnParam->childNode == NULL)
					returnParamType = INVALID;
				else
					returnParamType = findTypeInSymbolTable(funcSymbolTable,returnParam->childNode->data.token_data,returnParam->childNode->data.lineNumber,returnParam->childNode->data.charNumber);
				if(returnParamType == INVALID)
				{
					// There was an error. Move on to the next statement.
					return;
				}
				if(returnParamType != convertToType(returnParams->data->type))
				{
					// Type mismatch. Move on to next statement.
					semanticError(7,returnParams->data->value,returnParam->childNode->data.lineNumber,returnParam->childNode->data.charNumber);
					return;
				}
			}
			returnParams = returnParams->nextEntry;
			returnList = returnList->childNode->sisterNode;
			if(returnList->ruleNum == 33)
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
				semanticError(6,funcSymbolTable->functionName,rhsNodes->childNode->childNode->data.lineNumber,rhsNodes->childNode->childNode->data.charNumber);
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
						inputParamType = checkTypeInSymbolTable(inputParam->childNode->data.token_data,inputParam->childNode->data.lineNumber,inputParam->childNode->data.charNumber);
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
					semanticError(7,inputParams->data->value,inputParam->childNode->data.lineNumber,inputParam->childNode->data.charNumber);
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
			semanticError(8,"",inputParaList->childNode->childNode->childNode->data.lineNumber,inputParaList->childNode->childNode->childNode->data.lineNumber);
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
			if(LHSNodes->ruleNum == 33)
				LHSNodes = NULL;
			else
			{
				LHSNodes = LHSNodes->childNode;
			}
		}
	}
}

/**
 * Analyze the new function seen and run semantic checker
 * on the lines within the function.
 */
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
	// popTableInSemanticAnalyzer();
}

/**
 * Go to the new else scope and push it's symbol table
 * to the current symbol table.
 */
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

/**
 * Analyze statements within the new if block for semantic
 * errors.
 */
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

/**
 * Go to the next statement to be analyzed.
 */
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

/**
 * Separate function to run semantic analyzer in the if 
 * scope.
 */
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

/**
 * Run semantic analyzer when in a function scope.
 */
void runSemanticAnalyzer()
{
	while(currentASTNode != NULL)
	{
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

/**
 * Main function called by driver to run semantic analyzer
 * on the entire code and show errors if any.
 * @param  astRoot
 * @param  headList
 * @return
 */
int semanticAnalyzer(astTree astRoot, STList headList)
{
	curScope = 0;
	scopeIdentifier = 0;
	returnParamsAssignedValue = 0;
	currentASTNode = astRoot->childNode->childNode;
	stList = headList;
	runSemanticAnalyzer();
	return 0;
}