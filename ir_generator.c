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
#include "ir_generator.h"

astTree currentASTNode;
STList stList;
IRTable currentIR;
IRTable headIR;
int curTempVar;
int lineNum;

void initialiseIRTable()
{
	currentIR = malloc(sizeof(struct _IRTable));
	currentIR->lineNum = 0;
	lineNum = 1;
	currentIR->nextEntry = NULL;
	headIR = currentIR;
	curTempVar = 0;
}

int getLengthFromStringTable(char* name)
{
	stringSizes entry = stList->stringTable;
	while(strcmp(entry->stringName,name))
	{
		entry = entry->nextEntry;
		if(entry->stringName == NULL)
			break;
	}
	return entry->length;
}

IRItem findTempNodeForID(char* name)
{
	IRTable findIR = headIR;
	while(findIR != NULL)
	{
		if(findIR->arg1->entry != NULL)
		{
			if(!strcmp(findIR->arg1->entry,name) && !findIR->arg1->isSize)
				return findIR->arg1;
		}
		if(findIR->arg2 != NULL && findIR->arg2->entry != NULL)
		{
			if(!strcmp(findIR->arg2->entry,name) && !findIR->arg2->isSize)
				return findIR->arg2;
		}
		if(findIR->result != NULL && findIR->result->entry != NULL)
		{
			if(!strcmp(findIR->result->entry,name) && !findIR->result->isSize)
				return findIR->result;
		}
		findIR = findIR->nextEntry;
	}
	return NULL;
}

matrixSizes findMatrixInMatrixTable(char* name)
{
	matrixSizes entry = stList->matrixTable;
	while(strcmp(entry->matrixName,name))
	{
		entry = entry->nextEntry;
		if(entry->stringName == NULL)
			break;
	}
	return entry;
}

IRItem findTempNodeForMatrixElement(char* name, char* row, char* col)
{
	int rows = atoi(row);
	int column = atoi(col);
	IRTable findIR = headIR;
	while(findIR != NULL)
	{
		if(findIR->arg1->entry != NULL)
		{
			if(!strcmp(findIR->arg1->entry,name) && findIR->arg1->isSize && findIR->arg1->rowNum == rows && findIR->arg1->colNum == column)
				return findIR->arg1;
		}
		if(findIR->arg2 != NULL && findIR->arg2->entry != NULL)
		{
			if(!strcmp(findIR->arg2->entry,name) && findIR->arg2->isSize && findIR->arg2->rowNum == rows && findIR->arg2->colNum == column)
				return findIR->arg2;
		}
		if(findIR->result != NULL && findIR->result->entry != NULL)
		{
			if(!strcmp(findIR->result->entry,name) && findIR->result->isSize && findIR->result->rowNum == rows && findIR->result->colNum == column)
				return findIR->result;
		}
		findIR = findIR->nextEntry;
	}
	return NULL;
}

token getTypeFromSymbolTable(char* name)
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
				STError();
				entry = NULL;
				break;
			}
			entry = stList->parentList->table;
		}
	}
	return convertType(entry->data->type);
}

IRItem createIRForArithmeticTerm(IRItem leftVar, astTree node)
{
	astTree term = node->childNode->sisterNode;
	astTree op = node->childNode;
	astTree factor = term->childNode;
	IRItem var1 = malloc(sizeof(struct _IRItem));
	IRItem returnItem = malloc(sizeof(struct _IRItem));
	char tempName[3];
	sprintf(tempName,"t%d",curTempVar++);
	returnItem->temp_name = tempName;
	if(factor->ruleNum == 48)
	{
		// It is just a variable. Assign it an IR node.
		IRItem item = malloc(sizeof(struct _IRItem));
		astTree varNode = factor->childNode;
		if(varNode->ruleNum == 65)
		{
			item = findTempNodeForID(varNode->data.token_data);
			if(varNode->childNode->sisterNode->ruleNum == 78)
			{
				if(item != NULL)
				{
					var1 = item;
				}
				else
				{
					// Temp node for variable does not exist.
					var1->entry = varNode->data.token_data;
					sprintf(tempName,"t%d",curTempVar++);
					var1->temp_name = tempName;
				}
			}
			else
			{
				// It is a matrix element node.
				item = findTempNodeForMatrixElement(varNode->data.token_data,varNode->sisterNode->childNode->data.token_data,varNode->sisterNode->childNode->sisterNode->data.token_data);
				if(item != NULL)
				{
					var1 = item;
				}
				else
				{
					// Temp node for matrix element does not exist
					var1->entry = varNode->data.token_data;
					sprintf(tempName,"t%d",curTempVar++);
					var1->temp_name = tempName;
					var1->typeName = MAT;
					var1->isSize = TRUE;
					var1->rowNum = atoi(varNode->sisterNode->childNode->data.token_data); 
					var1->colNum = atoi(varNode->sisterNode->childNode->sisterNode->data.token_data);
				}
			}
		}
		else if(varNode->ruleNum == 66)
		{
			// Number element.
			var1->entry = NULL;
			var1->temp_name = tempName;
			var1->typeName = INTEGER;
			var1->value = atoi(varNode->childNode->data.token_data);
			var1->strValue = NULL;
		}
		else if(varNode->ruleNum == 67)
		{
			var1->entry = NULL;
			var1->temp_name = tempName;
			var1->typeName = FLOAT;
			var1->floatValue = atof(varNode->childNode->data.token_data);
			var1->strValue = NULL;
		}
		else if(varNode->ruleNum == 68)
		{
			var1->entry = NULL;
			var1->temp_name = tempName;
			var1->typeName = STRINGENUM;
			var1->strValue = varNode->childNode->data.token_data;
			var1->strValue = NULL;
		}
	}
	else
	{
		// Arithmetic Expression.
		var1 = createIRForArithmeticExpression(factor->childNode);
	}
	astTree varNode2 = factor->sisterNode;
	if(varNode2->ruleNum == 54)
	{
		// End of operators.
		if(op->ruleNum == 57)
			currentIR->op = MUL;
		else
			currentIR->op = DIV;
		currentIR->result = returnItem;
		currentIR->arg1 = leftVar;
		currentIR->arg2 = var1;
		currentIR->lineNum = lineNum++;
		currentIR->nextEntry = malloc(sizeof(struct _IRTable));
		currentIR = currentIR->nextEntry;
	}
	else
	{
		
	}
}

IRItem createIRForArithmeticExpression(astTree node)
{
	astTree term = node->childNode;
	astTree factor = term->childNode;
	IRItem var1 = malloc(sizeof(struct _IRItem));
	IRItem returnItem = malloc(sizeof(struct _IRItem));
	char tempName[3];
	sprintf(tempName,"t%d",curTempVar++);
	returnItem->temp_name = tempName;
	if(factor->ruleNum == 48)
	{
		// It is just a variable. Assign it an IR node.
		IRItem item = malloc(sizeof(struct _IRItem));
		astTree varNode = factor->childNode;
		if(varNode->ruleNum == 65)
		{
			item = findTempNodeForID(varNode->data.token_data);
			if(varNode->childNode->sisterNode->ruleNum == 78)
			{
				if(item != NULL)
				{
					// Item already exists.
					var1 = item;
				}
				else
				{
					// Temp node for variable does not exist.
					var1->entry = varNode->data.token_data;
					sprintf(tempName,"t%d",curTempVar++);
					var1->temp_name = tempName;
				}
			}
			else
			{
				// It is a matrix element node.
				item = findTempNodeForMatrixElement(varNode->data.token_data,varNode->sisterNode->childNode->data.token_data,varNode->sisterNode->childNode->sisterNode->data.token_data);
				if(item != NULL)
				{
					var1 = item;
				}
				else
				{
					// Temp node for matrix element does not exist
					var1->entry = varNode->data.token_data;
					sprintf(tempName,"t%d",curTempVar++);
					var1->temp_name = tempName;
					var1->typeName = MAT;
					var1->isSize = TRUE;
					var1->rowNum = atoi(varNode->sisterNode->childNode->data.token_data); 
					var1->colNum = atoi(varNode->sisterNode->childNode->sisterNode->data.token_data);
				}
			}
		}
		else if(varNode->ruleNum == 66)
		{
			// Number element.
			var1->entry = NULL;
			var1->temp_name = tempName;
			var1->typeName = INTEGER;
			var1->value = atoi(varNode->childNode->data.token_data);
			var1->strValue = NULL;
		}
		else if(varNode->ruleNum == 67)
		{
			var1->entry = NULL;
			var1->temp_name = tempName;
			var1->typeName = FLOAT;
			var1->floatValue = atof(varNode->childNode->data.token_data);
			var1->strValue = NULL;
		}
		else if(varNode->ruleNum == 68)
		{
			var1->entry = NULL;
			var1->temp_name = tempName;
			var1->typeName = STRINGENUM;
			var1->strValue = varNode->childNode->data.token_data;
			var1->strValue = NULL;
		}
		// Matrix additions being handled elsewhere.
	}
	else
	{
		// Arithmetic Expression.
		var1 = createIRForArithmeticExpression(factor->childNode);
	}

	// Check the other node.
	astTree varNode2 = term->childNode->sisterNode;
	if(varNode2->ruleNum == 53)
	{
		// High precedence operator rules.
		var1 = createIRForArithmeticTerm(var1,varNode2);
	}

	varNode2 = term->sisterNode;
	if(varNode2->ruleNum == 51)
	{
		// There is no low precendence operator rules.
		currentIR->op = MOV;
		currentIR->result = returnItem;
		currentIR->arg1 = var1;
		currentIR->lineNum = lineNum++;
		currentIR->arg2 = NULL;
		currentIR->nextEntry = malloc(sizeof(struct _IRTable));
		currentIR = currentIR->nextEntry;
	}
	else
	{
		// There is a low precedence operator.
		IRTable curIR = currentIR;
		if(varNode2->childNode->ruleNum == 55)
		{
			curIR->op = ADD;
		}
		else
		{
			curIR->op = SUB;
		}
		curIR->nextEntry = malloc(sizeof(struct _IRTable));
		currentIR = curIR->nextEntry;
		IRItem var2 = malloc(sizeof(struct _IRItem));
		var2 = createIRForArithmeticExpression(varNode2->childNode->sisterNode);
		curIR->result = returnItem;
		curIR->arg1 = var1;
		curIR->arg2 = var2;
		// Reset this to the current entry.
		curIR = currentIR;
		curIR->nextEntry = malloc(sizeof(struct _IRTable));
		currentIR = curIR->nextEntry;
	}

	return returnItem;
}

void createIRForAssignmentStmt()
{
	// arithmetic Expression or size Expression.
	astTree stmtNode = currentASTNode->childNode;
	if(stmtNode->childNode->sisterNode->ruleNum == 27)
	{
		// Size Expression.
		astTree idNode = stmtNode->childNode->sisterNode->childNode->childNode; // ruleNum 34 ID.
		currentIR->op = MOV;
		token type = getTypeFromSymbolTable(idNode->data.token_data);
		if(type == STR)
		{
			// Add one node containing the value of string length.
			int length = getLengthFromStringTable(idNode->data.token_data);
			currentIR->lineNum = lineNum++;
			currentIR->arg1 = malloc(sizeof(struct _IRItem));
			currentIR->result = malloc(sizeof(struct _IRItem));
			sprintf(currentIR->arg1->temp_name,"t%d",curTempVar++);
			currentIR->arg1->value = length;
			currentIR->arg1->typeName = INTEGER;
			currentIR->arg1->isSize = TRUE;
			currentIR->arg1->jumpStmt = FALSE;
			currentIR->arg1->entry = idNode->data.token_data;
			astTree assignNode = stmtNode->childNode->childNode;
			char tempName[3];
			IRItem item = findTempNodeForID(assignNode->data.token_data);
			if(item != NULL)
			{
				// The item already exists in the temp nodes.
				currentIR->result->temp_name = item->temp_name;
			}
			else
			{
				// Need to create a new temp name.
				sprintf(tempName,"t%d",curTempVar++);
				currentIR->result->temp_name = tempName;
			}
			currentIR->result->entry = assignNode->data.token_data;
			currentIR->result->isSize = FALSE;
			currentIR->result->typeName = INTEGER;
			currentIR->result->jumpStmt = FALSE;
			currentIR->arg2 = NULL;
			currentIR->nextEntry = malloc(sizeof(struct _IRTable));
			currentIR = currentIR->nextEntry;
		}
	}
	else
	{
		// Arithmetic Expression.
		// Assign a name to the variable being assigned the value.
		astTree assignNode = stmtNode->childNode->childNode;
		char tempName[3];
		IRItem item = findTempNodeForID(assignNode->data.token_data);
		currentIR->result = malloc(sizeof(struct _IRItem));
		token type = getTypeFromSymbolTable(assignNode->data.token_data);
		currentIR->result->typeName = INTEGER;
		if(type == RNUM)
		{
			currentIR->result->typeName = FLOAT;
		}
		else if(type == STRINGENUM)
		{
			currentIR->result->typeName = STRINGENUM;
		}
		else if(type == MAT)
		{
			// Addition for each element of the matrix. :/
		}
		if(item != NULL)
		{
			currentIR->result->temp_name = item->temp_name;
		}
		else
		{
			// Create a new temp name.
			sprintf(tempName,"t%d",curTempVar++);
			currentIR->result->temp_name = tempName;
		}
		astTree rhsNodes = stmtNode->childNode->sisterNode;
		astTree arithNode = rhsNodes->childNode;
		IRItem finalNode = createIRForArithmeticExpression(arithNode);
	}
}

void createIRForAssignmentStmtType2()
{
	astTree stmtNode = currentASTNode->childNode;
	if(stmtNode->childNode->sisterNode->ruleNum == 29)
	{
		// Size Expression.
		astTree idNode = stmtNode->childNode->sisterNode->childNode->childNode; // ruleNum 34 ID.
		currentIR->op = MOV;
		token type = getTypeFromSymbolTable(idNode->data.token_data);
		if(type == MAT)
		{
			// It is a matrix.
			// Add two nodes containing the value of matrix lengths.
			matrixSizes matrix = findMatrixInMatrixTable(idNode->data.token_data);
			currentIR->lineNum = lineNum++;
			currentIR->arg1 = malloc(sizeof(struct _IRItem));
			currentIR->result = malloc(sizeof(struct _IRItem));
			sprintf(currentIR->arg1->temp_name,"t%d",curTempVar++);
			currentIR->arg1->value = matrix->rows;
			currentIR->arg1->isSize = TRUE;
			currentIR->arg1->jumpStmt = FALSE;
			currentIR->arg1->entry = idNode->data.token_data;
			astTree assignNode = stmtNode->childNode->childNode;
			char tempName[3];
			IRItem item = findTempNodeForID(assignNode->data.token_data);
			if(item != NULL)
			{
				// The item already exists in the temp nodes.
				currentIR->result->temp_name = item->temp_name;
			}
			else
			{
				// Need to create a new temp name.
				sprintf(tempName,"t%d",curTempVar++);
				currentIR->result->temp_name = tempName;
			}
			currentIR->result->entry = assignNode->data.token_data;
			currentIR->result->isSize = FALSE;
			currentIR->result->jumpStmt = FALSE;
			currentIR->arg2 = NULL;
			currentIR->nextEntry = malloc(sizeof(struct _IRTable));

			// Column value.
			currentIR = currentIR->nextEntry;
			currentIR->arg1 = malloc(sizeof(struct _IRItem));
			currentIR->result = malloc(sizeof(struct _IRItem));
			sprintf(currentIR->arg1->temp_name,"%d",matrix->columns);
			currentIR->arg1->isSize = TRUE;
			currentIR->arg1->jumpStmt = FALSE;
			currentIR->arg1->entry = idNode->data.token_data;
			item = findTempNodeForID(assignNode->data.token_data);
			if(item != NULL)
			{
				// The item already exists in the temp nodes.
				currentIR->result->temp_name = item->temp_name;
			}
			else
			{
				// Need to create a new temp name.
				sprintf(tempName,"t%d",curTempVar++);
				currentIR->result->temp_name = tempName;
			}
			currentIR->result->entry = assignNode->data.token_data;
			currentIR->result->isSize = FALSE;
			currentIR->result->jumpStmt = FALSE;
			currentIR->arg2 = NULL;
			currentIR->nextEntry = malloc(sizeof(struct _IRTable));
			currentIR = currentIR->nextEntry;
		}
	}
}

void createIRForNextStatement()
{
	if(currentASTNode == NULL)
		return;
	if(currentASTNode->ruleNum == 6)
	{
		// It is a nullable.
		// End of program. Stop.
		currentASTNode = NULL;
		return;
	}
	else
	{
		// More statements exist.
		currentASTNode = currentASTNode->childNode->childNode;
		return;
	}
}

void createIRCode()
{
	while(currentASTNode != NULL)
	{
		if(currentASTNode->ruleNum == 4)
		{
			// It is a function.
			// That isn't allowed.
		}
		else
		{
			currentASTNode = currentASTNode->childNode;
			switch(currentASTNode->ruleNum)
			{
				case 7: break; // declaration stmt.
				case 8: createIRForAssignmentStmt();break;
				case 9: createIRForAssignmentStmtType2();break;
				case 10: createIRForIfStmt();break;
				case 11: createIRForIOStmt();break; // ioStmt
				case 12: break; // funCall stmt.
			}
		}
		currentASTNode = currentASTNode->parentNode->sisterNode;
		createIRForNextStatement();
	}
}

int irGenerator(astTree astRoot, STList headList)
{
	// No need to maintain scopes as there is only one true function.
	initialiseIRTable();
	stList = headList;
	currentASTNode = astRoot->childNode->childNode;
	createIRCode();
	printf("IR code generated\n");
	return 0;
}