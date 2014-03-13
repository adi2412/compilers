// Automated first and follow set generation.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parserDef.h"
#include "lexer.h"

int getNonTerminal(char* term)
{
	// Check for all the possible types of non terminals
	if(!strcmp(term, "[ mainFunction ]")) return mainFunction;
	else if(!strcmp(term, "[ stmtsAndFunctionDefs ]")) return stmtsAndFunctionDefs;
	else if(!strcmp(term, "[ stmtOrFunctionDef ]")) return stmtOrFunctionDef;
	else if(!strcmp(term, "[ stmt ]")) return stmt;
	else if(!strcmp(term, "[ functionDef ]")) return functionDef;
	else if(!strcmp(term, "[ parameter_list ]")) return parameter_list;
	else if(!strcmp(term, "[ type ]")) return type;
	else if(!strcmp(term, "[ remainingList ]")) return remainingList;
	else if(!strcmp(term, "[ declarationStmt ]")) return declarationStmt;
	else if(!strcmp(term, "[ var_list ]")) return var_list;
	else if(!strcmp(term, "[ more_ids ]")) return more_ids;
	else if(!strcmp(term, "[ assignmentStmt_type1 ]")) return assignmentStmt_type1;
	else if(!strcmp(term, "[ assignmentStmt_type2 ]")) return assignmentStmt_type2;
	else if(!strcmp(term, "[ leftHandSide_singleVar ]")) return leftHandSide_singleVar;
	else if(!strcmp(term, "[ leftHandSide_listVar ]")) return leftHandSide_listVar;
	else if(!strcmp(term, "[ rightHandSide_type1 ]")) return rightHandSide_type1;
	else if(!strcmp(term, "[ rightHandSide_type2 ]")) return rightHandSide_type2;
	else if(!strcmp(term, "[ sizeExpression ]")) return sizeExpression;
	else if(!strcmp(term, "[ ifStmt ]")) return ifStmt;
	else if(!strcmp(term, "[ otherStmts ]")) return otherStmts;
	else if(!strcmp(term, "[ ioStmt ]")) return ioStmt;
	else if(!strcmp(term, "[ funCallStmt ]")) return funCallStmt;
	else if(!strcmp(term, "[ inputParameterList ]")) return inputParameterList;
	else if(!strcmp(term, "[ listVar ]")) return listVar;
	else if(!strcmp(term, "[ arithmeticExpression ]")) return arithmeticExpression;
	else if(!strcmp(term, "[ arithmeticTerm ]")) return arithmeticTerm;
	else if(!strcmp(term, "[ factor ]")) return factor;
	else if(!strcmp(term, "[ operator_lowPrecedence ]")) return operator_lowPrecedence;
	else if(!strcmp(term, "[ operator_highPrecedence ]")) return operator_highPrecedence;
	else if(!strcmp(term, "[ booleanExpression ]")) return booleanExpression;
	else if(!strcmp(term, "[ constrainedVars ]")) return constrainedVars;
	else if(!strcmp(term, "[ var ]")) return var;
	else if(!strcmp(term, "[ matrix ]")) return matrix;
	else if(!strcmp(term, "[ rows ]")) return rows;
	else if(!strcmp(term, "[ row ]")) return row;
	else if(!strcmp(term, "[ remainingColElements ]")) return remainingColElements;
	else if(!strcmp(term, "[ matrixElement ]")) return matrixElement;
	else if(!strcmp(term, "[ logicalOp ]")) return logicalOp;
	else if(!strcmp(term, "[ relationalOp ]")) return relationalOp;
	else 
	{
		//printf("invalid non temrinal: %s", term);
		return invalidNonTerm; // Handle such errors
	}
}

token getToken(char* term)
{
	// Check for all possible types of terminals
	if(!strcmp(term, "ASSIGNOP")) return ASSIGNOP;
	else if(!strcmp(term, "COMMENT")) return COMMENT;
	else if(!strcmp(term, "FUNID")) return FUNID;
	else if(!strcmp(term, "ID")) return ID;
	else if(!strcmp(term, "NUM")) return NUM;
	else if(!strcmp(term, "RNUM")) return RNUM;
	else if(!strcmp(term, "STR")) return STR;
	else if(!strcmp(term, "END")) return END;
	else if(!strcmp(term, "INT")) return INT;
	else if(!strcmp(term, "REAL")) return REAL;
	else if(!strcmp(term, "STRING")) return STRING;
	else if(!strcmp(term, "MATRIX")) return MATRIX;
	else if(!strcmp(term, "MAIN")) return MAIN;
	else if(!strcmp(term, "SQO")) return SQO;
	else if(!strcmp(term, "SQC")) return SQC;
	else if(!strcmp(term, "OP")) return OP;
	else if(!strcmp(term, "CL")) return CL;
	else if(!strcmp(term, "SEMICOLON")) return SEMICOLON;
	else if(!strcmp(term, "COMMA")) return COMMA;
	else if(!strcmp(term, "IF")) return IF;
	else if(!strcmp(term, "ELSE")) return ELSE;
	else if(!strcmp(term, "ENDIF")) return ENDIF;
	else if(!strcmp(term, "READ")) return READ;
	else if(!strcmp(term, "PRINT")) return PRINT;
	else if(!strcmp(term, "PLUS")) return PLUS;
	else if(!strcmp(term, "MINUS")) return MINUS;
	else if(!strcmp(term, "MUL")) return MUL;
	else if(!strcmp(term, "DIV")) return DIV;
	else if(!strcmp(term, "SIZE")) return SIZE;
	else if(!strcmp(term, "AND")) return AND;
	else if(!strcmp(term, "OR")) return OR;
	else if(!strcmp(term, "NOT")) return NOT;
	else if(!strcmp(term, "LT")) return LT;
	else if(!strcmp(term, "LE")) return LE;
	else if(!strcmp(term, "GT")) return GT;
	else if(!strcmp(term, "GE")) return GE;
	else if(!strcmp(term, "EQ")) return EQ;
	else if(!strcmp(term, "NE")) return NE;
	else if(!strcmp(term, "FUNCTION")) return FUNCTION;
	else if(!strcmp(term, "?")) return NIL;
	else{
		//printf("invalid term: %s",term);
		return INVALID; // Handle such errors better
	} 
}

terminal readRHSTerm(char* term, char* ruleString, int ptr)
{
	terminal RHSTerminal;
	int i = 0;
	if(ptr >= strlen(ruleString)-3)
	{
		// Exceeded too far.
		RHSTerminal.nontermValue = invalidNonTerm;
		RHSTerminal.tokValue = INVALID;
		return RHSTerminal;
	}
	if(ruleString[ptr] == '[')
	{
		RHSTerminal.flag = TRUE;
		// It is a non terminal
		while(ruleString[ptr] != ']')
		{
			term[i++] = ruleString[ptr++];
		}
		term[i] = ruleString[ptr];
		term[++i] = '\0';
		//printf("debug from non terminal: %s\n",term);
		// Assign the value of enumerated non Terminal
		RHSTerminal.nontermValue = getNonTerminal(term);
		RHSTerminal.tokValue = INVALID;
	}
	else
	{
		RHSTerminal.flag = FALSE;
		// It is a terminal
		while(ruleString[ptr] != ' ')
		{
			term[i++] = ruleString[ptr++];

		}
		term[i] = '\0';
		//printf("debug from terminal: %s\n",term);
		RHSTerminal.tokValue = getToken(term);
		RHSTerminal.nontermValue = invalidNonTerm;
	}
	return RHSTerminal;
}

terminals computeRHSTerm(char* ruleString, int ptr)
{
	terminals RHSTerms = malloc(sizeof(struct _termSet));
	terminals headRHSTerms = RHSTerms;
	char* RHSTerm = malloc(NONTERMSIZE*sizeof(char)); // The longest RHS Term could be a non terminal
	// Read and return first RHS term
	terminal RHSTerminal = readRHSTerm(RHSTerm, ruleString, ptr);
	RHSTerms->term_value = RHSTerminal;
	if(RHSTerms->term_value.tokValue == NIL)
	{
		RHSTerms->nullable = TRUE;
		RHSTerms->nextTerm = NULL;
		return RHSTerms;
	}
	RHSTerms->nullable = FALSE;
	RHSTerms->nextTerm = malloc(sizeof(struct _termSet));
	ptr = ptr + 1 + strlen(RHSTerm);
	//printf("first time size of ptr: %d\n",ptr);
	RHSTerm[0] = '\0';
	while(RHSTerms != NULL)
	{
		//printf("rhsterm: %s\n",RHSTerm);
		RHSTerms->nextTerm = malloc(sizeof(struct _termSet));
		RHSTerms->nextTerm->term_value = readRHSTerm(RHSTerm, ruleString, ptr);
		if((RHSTerms->nextTerm->term_value.nontermValue == invalidNonTerm) && (RHSTerms->nextTerm->term_value.tokValue == INVALID))
		{
			RHSTerms->nextTerm = NULL;
			return headRHSTerms;
		}
		if(RHSTerms->nextTerm->term_value.tokValue == NIL)
		{
			RHSTerms->nullable = TRUE;
			RHSTerms->nextTerm->nextTerm = NULL;
			return headRHSTerms;
		}
		RHSTerms->nullable = FALSE;
		RHSTerms = RHSTerms->nextTerm;
		//printf("size of character: %s\n",RHSTerm);
		ptr = ptr + 1 + strlen(RHSTerm);
		//printf("loop time size of ptr: %d",ptr);
		RHSTerm[0] = '\0';
	}

	// All RHS terms read
	return headRHSTerms;
}

char* readProductionTerm(char* term, char* rule)
{
	int i = 0;
	while(rule[i] != ']')
	{
		term[i] = rule[i];
		++i;
	}
	term[i] = ']';
	return term;
}

rule readAndDefineGrammarRule(FILE *fp)
{
	// Reads the file and creates a single rule.
	char ruleString[RULESIZE];
	if(fgets(ruleString,RULESIZE,fp) == NULL)
	{
		// All rules have been read.
		return NULL;
	}

	char* productionTerm = malloc(NONTERMSIZE*sizeof(char));
	// Read the production term and get the enumerated value
	readProductionTerm(productionTerm, ruleString);
	nonTerminal nonterm_value = getNonTerminal(productionTerm);
	rule gramRule = malloc(sizeof(struct _rule));
	gramRule->nonterm_value = nonterm_value;

	// Read the RHS terms.
	int pointer = 0;
	pointer = OFFSET + strlen(productionTerm);
	// Compute the first non terminal
	terminals RHSTerm = computeRHSTerm(ruleString,pointer);
	gramRule->nullable = RHSTerm->nullable;
	gramRule->termSet = RHSTerm;
	return gramRule;
}

int main()
{
	// Creates all the first sets for all the non-terminals
	FILE *fp = fopen("grammar_rules.txt","r");
	if(fp == NULL)
	{
		printf("The file could not be found.");
		exit(1);
	}
	// Read the first grammar rule
	rule gramRule = readAndDefineGrammarRule(fp);
	int ruleNum = 1;
	// Maintain a pointer at the first rule
	rule headRule = gramRule;
	while(gramRule != NULL)
	{
		// Keep computing first sets and reading grammar rules
		gramRule->ruleNum = ruleNum++;
		gramRule->nextRule = readAndDefineGrammarRule(fp);
		printf("Rule Number %d) %d-->",gramRule->ruleNum,gramRule->nonterm_value);
		terminals readterms = gramRule->termSet;
		while(readterms != NULL)
		{
			if(readterms->term_value.flag)
			{
				// Non terminal
				printf("%d\t",readterms->term_value.nontermValue);
			}
			else
			{
				printf("%d\t",readterms->term_value.tokValue);
			}
			readterms = readterms->nextTerm;
		}
		printf("\n");
		gramRule = gramRule->nextRule;
	}

	// // Having read all the rules, start computing first sets.
	// gramRule = headRule;
	// computeFirst(gramRule);
	// while(!checkIfAllFirsts(gramRule))
	// {
	// 	// All firsts not made. gramRule is pointing at the rule whose FIRST has not been computed
	// 	computeFirst(gramRule);
	// }

	// // Make the follow sets.
	// gramRule = headRule;
	// computeFollow(gramRule);
	// while(!checkIfAllFollow(gramRule))
	// {
	// 	// All follows not made. gramRule is pointing at the rule whose FOLLOW has not been computed.
	// 	computeFollow(gramRule);
	// }

	return 0;
}
