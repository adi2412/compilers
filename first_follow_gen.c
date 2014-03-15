// Automated first and follow set generation.
/* Compilers Project Group 2
// Aditya Raisinghani 2011A7PS042P
// Utkarsh Verma 2011A7PS137P
// BITS Pilani, Pilani Campus
// Second semester 2014
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parserDef.h"
#include "lexer.h"


rule headRule;

first computeFirst(rule gramRule, nonterm head);

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
	RHSTerm[0] = '\0';
	while(RHSTerms != NULL)
	{
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
		ptr = ptr + 1 + strlen(RHSTerm);
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

nonterm initialiseNonTerminals()
{
	// Initialise all pointers to NULL.
	int i = 0;
	nonterm nonTermHead = malloc(NONTERMINALS*sizeof(struct _nonterm*));
	for(;i<NONTERMINALS;++i)
	{
		nonTermHead[i] = NULL;
	}
	// Initialise their first and follow sets to NULL.
	// for(i=0;i<NONTERMINALS;++i)
	// {
	// 	int j;
	// 	for(j=0;j<TERMINALS;++j)
	// 	{
	// 		nonTermHead[i]->firstSet[j] = NULL;
	// 		nonTermHead[i]->followSet[j] = NULL;
	// 	}
	// }
	return nonTermHead;
}

first addFirsts(first mainSet, first copySet, rule gramRule)
{
	int i;
	for(i=0;i<TERMINALS;++i)
	{
		if(copySet[i] != NULL)
		{
			mainSet[i] = malloc(sizeof(struct _first));
			mainSet[i]->token_name = copySet[i]->token_name;
			mainSet[i]->ruleNum = gramRule->ruleNum;
		}
	}

	return mainSet;
}

first makeFirstUsingTerm(rule gramRule, nonterm head, terminals terms)
{
	nonTerminal nonTerm = gramRule->nonterm_value;
	if(head[nonTerm] == NULL)
	{
		head[nonTerm] = malloc(sizeof(struct _nonterm));
	}
	// Check to see if terms exist.
	if(terms == NULL)
	{
		return head[nonTerm]->firstSet;
	}
	if(!(terms->term_value.flag))
	{
		// It is a terminal. Just assign this to the firstSet.
		token tokValue = terms->term_value.tokValue;
		if(head[nonTerm]->firstSet == NULL)
		{
			head[nonTerm]->firstSet = malloc(TERMINALS*sizeof(struct _first*));
		}
		head[nonTerm]->firstSet[tokValue] = malloc(sizeof(struct _first));
		head[nonTerm]->firstSet[tokValue]->token_name = tokValue;
		head[nonTerm]->firstSet[tokValue]->ruleNum = gramRule->ruleNum;
		if(tokValue == NIL)
		{
			// It is a nil value. Keep adding next term to first set.
			head[nonTerm]->nullable = TRUE;
			// CONSIDER: The grammar has single production rules. Which is why I am not considering even going ahead if this term is nil and just ending it here.
		}
		return head[nonTerm]->firstSet;
	}
	else
	{
		// It is a non terminal. Assign the first set of non terminal to this non terminal.
		nonTerminal otherNonTerm = terms->term_value.nontermValue;
		if(head[otherNonTerm] != NULL)
		{
			// The first set of this non terminal has already been computed. Just use.
			first firstSet = head[otherNonTerm]->firstSet;
			//int i;
			if(head[nonTerm]->firstSet == NULL)
			{
				head[nonTerm]->firstSet = malloc(TERMINALS*sizeof(struct _first*));
			}
			head[nonTerm]->firstSet = addFirsts(head[nonTerm]->firstSet,firstSet,gramRule);
			if(firstSet[NIL] != NULL)
			{
				// The first set of this non terminal consists of NULL production
				terms = terms->nextTerm;
				while(terms != NULL)
				{
					head[nonTerm]->firstSet = makeFirstUsingTerm(gramRule, head, terms);
					if(terms->term_value.flag)
					{
						// It is a non terminal
						if(head[(terms->term_value.nontermValue)]->firstSet[NIL] != NULL)
							terms = terms->nextTerm;
						else
						{
							// Non terminal does not consist of NULL production
							head[nonTerm]->nullable = FALSE;
							return head[nonTerm]->firstSet;
						}
					}
					else
					{
						// It is a terminal. Return back.
						head[nonTerm]->nullable = FALSE;
						return head[nonTerm]->firstSet;
					}
				}

				// All the RHS terms have been exhausted.
				head[nonTerm]->nullable = TRUE;
				return head[nonTerm]->firstSet;
			}
			// Non terminal does not contain null production. Return directly.
			return head[nonTerm]->firstSet;
		}
		else
		{
			// First set of this non terminal has not been computed. Find and then use.
			// Find the first rule which has this non terminal on LHS.
			rule findRule = headRule;
			while(1)
			{
				if((findRule->nonterm_value) == otherNonTerm) break;

				findRule = findRule->nextRule;
			}
			while((findRule->nonterm_value) == otherNonTerm)
			{
				head[otherNonTerm]->firstSet = computeFirst(findRule, head);
				findRule = findRule->nextRule;
			}

			// First set computed. Use to create first of the non terminal.
			head[nonTerm]->firstSet = makeFirstUsingTerm(gramRule, head, terms);
		}
	}

	return head[nonTerm]->firstSet;
}

first computeFirst(rule gramRule, nonterm head)
{
	// Computes first sets for a specific non terminal.
	// CONSIDER: Need to also check that the non terminal of this rule has it's first computed already or not.
	nonTerminal nonTerm = gramRule->nonterm_value;
	if(head[nonTerm] == NULL)
	{
		head[nonTerm] = malloc(sizeof(struct _nonterm));
	}
	// nonTerminal exists but it's first doesn't. Compute the first
	//first firstSet = malloc(sizeof(struct _first));
	terminals terms = gramRule->termSet;
	head[nonTerm]->firstSet = makeFirstUsingTerm(gramRule, head, terms);

	return head[nonTerm]->firstSet;	// Return the existing non terminal itself.
}

follow addFollows(follow mainSet, follow copySet, rule gramRule)
{
	int i;
	for(i=0;i<TERMINALS-2;++i)
	{
		if(copySet[i] != NULL)
		{
			if(mainSet[i] == NULL)
			{
				mainSet[i] = malloc(sizeof(struct _first));
				mainSet[i]->token_name = copySet[i]->token_name;
				mainSet[i]->ruleNum = gramRule->ruleNum;
			}
		}
	}

	return mainSet;
}

follow addFollowsFromFirst(follow mainSet, first copySet, rule gramRule)
{
	int i;
	for(i=0;i<TERMINALS-2;++i)
	{
		if(copySet[i] != NULL)
		{
			if(mainSet[i] == NULL)
			{
				mainSet[i] = malloc(sizeof(struct _first));
				mainSet[i]->token_name = copySet[i]->token_name;
				mainSet[i]->ruleNum = gramRule->ruleNum;
			}
		}
	}

	return mainSet;
}

int checkNonTerminalInRule(rule checkRule, nonTerminal nonTerm)
{
	terminals terms = checkRule->termSet;
	while(terms != NULL)
	{
		if(terms->term_value.flag)
		{
			if(terms->term_value.nontermValue == nonTerm)
			{
			 	return 1;
			}
		}
		terms = terms->nextTerm;
	}
	return 0;
}

follow makeFollowForTerm(nonTerminal nonTerm, nonterm head)
{
	if(head[nonTerm]->followSet != NULL) return head[nonTerm]->followSet;
	//nonTerminal nonTerm = term;
	rule ruleList = headRule;
	head[nonTerm]->followSet = malloc(TERMINALS*sizeof(struct _follow*));
	while(ruleList != NULL)
	{
		if(checkNonTerminalInRule(ruleList, nonTerm))
		{
			terminals RHSTerms = ruleList->termSet;
			while(RHSTerms != NULL)
			{
				if(RHSTerms->term_value.flag)
				{
					if(RHSTerms->term_value.nontermValue == nonTerm)
					{
						break;
					}
				}
				RHSTerms = RHSTerms->nextTerm;
			}
			int flag = 0; // Flag to stop running for a particular rule.
			// This non terminal exists in RHS of this rule.
			while((RHSTerms->nextTerm != NULL) && !flag)
			{
				if(!(RHSTerms->nextTerm->term_value.flag))
				{
					// It is a terminal. Add to the follow set and leave.
					token tokValue = RHSTerms->nextTerm->term_value.tokValue;
					head[nonTerm]->followSet[tokValue] = malloc(sizeof(struct _follow));
					head[nonTerm]->followSet[tokValue]->token_name = tokValue;
					head[nonTerm]->followSet[tokValue]->ruleNum = ruleList->ruleNum;
					flag = 1;
					while(RHSTerms->nextTerm != NULL && flag)
					{
						// Check if this same non terminal possibly exists again.
						if(RHSTerms->nextTerm->term_value.flag)
						{
							if(RHSTerms->nextTerm->term_value.nontermValue == nonTerm)
							{
								flag = 0;
								break;
							}

						}
						RHSTerms = RHSTerms->nextTerm;
					}
				}
				else
				{
					// It is a non terminal.
					if(RHSTerms->nextTerm->term_value.nontermValue != nonTerm)
					{
						nonTerminal otherNonTerm = RHSTerms->nextTerm->term_value.nontermValue;
						head[nonTerm]->followSet = addFollowsFromFirst(head[nonTerm]->followSet,head[otherNonTerm]->firstSet,ruleList);
						if(!(head[otherNonTerm]->nullable)) flag = 1;
					}
					while(RHSTerms->nextTerm != NULL && flag)
					{
						// Check if this same non terminal possibly exists again.
						if(RHSTerms->nextTerm->term_value.flag)
						{
							if(RHSTerms->nextTerm->term_value.nontermValue == nonTerm) flag = 0;
						}
						RHSTerms = RHSTerms->nextTerm;
					}
				}
				if(RHSTerms->nextTerm != NULL)
				{
					RHSTerms = RHSTerms->nextTerm;
				}
			}
			if(RHSTerms->nextTerm == NULL && RHSTerms->term_value.flag)
			{
				if(RHSTerms->term_value.nontermValue == nonTerm)
				{
					// Last term. Compute follow of LHS and add to this non terminal.
					nonTerminal LHSNonTerm = ruleList->nonterm_value;
					if(head[LHSNonTerm]->followSet == NULL)
					{
						head[LHSNonTerm]->followSet = makeFollowForTerm(LHSNonTerm, head);
					}
					if(LHSNonTerm != nonTerm)
					{
						head[nonTerm]->followSet = addFollows(head[nonTerm]->followSet,head[LHSNonTerm]->followSet,ruleList);
					}
				}
			}
		}
		ruleList = ruleList->nextRule;
	}
	if(head[nonTerm]->nullable)
	{
		// It has a nullable. Find the nullable production.
		rule findRule = headRule;
		while(findRule != NULL)
		{
			if((findRule->nonterm_value) == nonTerm)
			{
				if(findRule->nullable) break;
			}
			findRule = findRule->nextRule;
		}
		int nullRuleNum = findRule->ruleNum;
		follow followTerms = head[nonTerm]->followSet;
		int i = 0;
		for(;i<TERMINALS-2;++i)
		{
			if(followTerms[i] != NULL)
			{
				head[nonTerm]->followSet[i]->ruleNum = nullRuleNum;
			}
		}
	}
	return head[nonTerm]->followSet;
}

follow computeFollow(rule gramRule, nonterm head)
{
	nonTerminal nonTerm = gramRule->nonterm_value;
	terminals terms = gramRule->termSet;
	while(terms != NULL)
	{
		if(terms->term_value.flag && (terms->term_value.nontermValue != nonTerm))
		{
			// It a non terminal. Compute it's follow
			head[terms->term_value.nontermValue]->followSet = makeFollowForTerm(terms->term_value.nontermValue, head);
			if(head[terms->term_value.nontermValue]->nullable)
			{
				// It has a nullable. Find the nullable production.
				rule findRule = headRule;
				while(findRule != NULL)
				{
					if((findRule->nonterm_value) == terms->term_value.nontermValue)
					{
						if(findRule->nullable) break;
					}
					findRule = findRule->nextRule;
				}
				int nullRuleNum = findRule->ruleNum;
				//follow followTerms = head[terms->term_value.nontermValue]->followSet;
				int i = 0;
				for(;i<TERMINALS;++i)
				{
					if(head[terms->term_value.nontermValue]->followSet[i] != NULL)
					{
						head[terms->term_value.nontermValue]->followSet[i]->ruleNum = nullRuleNum;
					}
				}
			}
		}
		terms = terms->nextTerm;
	}

	return head[nonTerm]->followSet;
}

char* getNonTermValue(nonTerminal nonterm)
{
	// Check for all the possible types of non terminals
	if(nonterm == mainFunction) return "mainFunction";
	else if(nonterm == stmtsAndFunctionDefs) return "stmtsAndFunctionDefs";
	else if(nonterm == stmtOrFunctionDef) return "stmtOrFunctionDef";
	else if(nonterm == stmt) return "stmt";
	else if(nonterm == functionDef) return "functionDef";
	else if(nonterm == parameter_list) return "parameter_list";
	else if(nonterm == type) return "type";
	else if(nonterm == remainingList) return "remainingList";
	else if(nonterm == declarationStmt) return "declarationStmt";
	else if(nonterm == var_list) return "var_list";
	else if(nonterm == more_ids) return "more_ids";
	else if(nonterm == assignmentStmt_type1) return "assignmentStmt_type1";
	else if(nonterm == assignmentStmt_type2) return "assignmentStmt_type2";
	else if(nonterm == leftHandSide_singleVar) return "leftHandSide_singleVar";
	else if(nonterm == leftHandSide_listVar) return "leftHandSide_listVar";
	else if(nonterm == rightHandSide_type1) return "rightHandSide_type1";
	else if(nonterm == rightHandSide_type2) return "rightHandSide_type2";
	else if(nonterm == sizeExpression) return "sizeExpression";
	else if(nonterm == ifStmt) return "ifStmt";
	else if(nonterm == otherStmts) return "otherStmts";
	else if(nonterm == ioStmt) return "ioStmt";
	else if(nonterm == funCallStmt) return "funCallStmt";
	else if(nonterm == inputParameterList) return "inputParameterList";
	else if(nonterm == listVar) return "listVar";
	else if(nonterm == arithmeticExpression) return "arithmeticExpression";
	else if(nonterm == arithmeticTerm) return "arithmeticTerm";
	else if(nonterm == factor) return "factor";
	else if(nonterm == operator_lowPrecedence) return "operator_lowPrecedence";
	else if(nonterm == operator_highPrecedence) return "operator_highPrecedence";
	else if(nonterm == booleanExpression) return "booleanExpression";
	else if(nonterm == constrainedVars) return "constrainedVars";
	else if(nonterm == var) return "var";
	else if(nonterm == matrix) return "matrix";
	else if(nonterm == rows) return "rows";
	else if(nonterm == row) return "row";
	else if(nonterm == remainingColElements) return "remainingColElements";
	else if(nonterm == matrixElement) return "matrixElement";
	else if(nonterm == logicalOp) return "logicalOp";
	else if(nonterm == relationalOp) return "relationalOp";
	else 
	{
		//printf("invalid non temrinal: %s", term);
		return "invalidNonTerm"; // Handle such errors
	}
}

int ffg(FILE* fp)
{
	// Creates all the first sets for all the non-terminal
	FILE *rules = fopen("grammar_rules.txt","r");
	if(rules == NULL)
	{
		printf("The grammar file could not be found.\n");
		exit(1);
	}

	//FILE *fp = fopen("grammarAndFirstFollowSet.txt","w");
	// Read the first grammar rule
	rule gramRule = readAndDefineGrammarRule(rules);
	int ruleNum = 1;
	// Maintain a pointer at the first rule
	headRule = gramRule;
	while(gramRule != NULL)
	{
		// Keep computing first sets and reading grammar rules
		gramRule->ruleNum = ruleNum++;
		gramRule->nextRule = readAndDefineGrammarRule(rules);
		fprintf(fp,"Rule Number %d) %s-->",gramRule->ruleNum,getNonTermValue(gramRule->nonterm_value));
		terminals readterms = gramRule->termSet;
		while(readterms != NULL)
		{
			if(readterms->term_value.flag)
			{
				// Non terminal
				fprintf(fp,"%s\t",getNonTermValue(readterms->term_value.nontermValue));
			}
			else
			{
				fprintf(fp,"%s\t",getTokenName(readterms->term_value.tokValue));
			}
			readterms = readterms->nextTerm;
		}
		fprintf(fp,"\n");
		gramRule = gramRule->nextRule;
	}
	// Having read all the rules, start computing first sets.
	gramRule = headRule;
	
	nonterm nonTerm = initialiseNonTerminals();
	int i = gramRule->nonterm_value;
	while(i<NONTERMINALS)
	{
		// First and follow still need to be computed for this non terminal.
		if(nonTerm[i] != NULL)
		{
			if(!nonTerm[i]->nullable) nonTerm[i]->nullable = FALSE;
		}
		if(nonTerm[i] == NULL)
		{
			nonTerm[i] = malloc(sizeof(struct _nonterm));
		}
		nonTerm[i]->firstSet = computeFirst(gramRule,nonTerm);
		gramRule = gramRule->nextRule;
		if(gramRule == NULL) break;
		i = gramRule->nonterm_value;
	}
	//fprintf(stderr,"first sets calculated");
	gramRule = headRule;
	// Compute the follow sets.
	i = gramRule->nonterm_value;
	// Set dollar in follow of start symbol
	nonTerm[i]->followSet = malloc(TERMINALS*sizeof(struct _follow*));
	nonTerm[i]->followSet[DOL] = malloc(sizeof(struct _follow*));
	nonTerm[i]->followSet[DOL]->token_name = DOL;
	nonTerm[i]->followSet[DOL]->ruleNum = gramRule->ruleNum;
	//fprintf(stderr,"first sets calculated");
	while(i<NONTERMINALS)
	{
		// All non terminals have been malloc'd. Just start finding their follow
		nonTerm[i]->followSet = computeFollow(gramRule, nonTerm);
		gramRule = gramRule->nextRule;
		if(gramRule == NULL) break;
		i = gramRule->nonterm_value;
	}

	int j = 0;
	for(;j<NONTERMINALS-1;++j)
	{
		fprintf(fp,"For %s ",getNonTermValue(j));
		if(nonTerm[j]->nullable)
		{
			fprintf(fp,"(NULLABLE)----->\n");
		}
		else
		{
			fprintf(fp,"----->\n");
		}
		fprintf(fp,"First set:\t");
		int k =0;
		for(;k<TERMINALS;++k)
		{
			if(nonTerm[j]->firstSet[k] != NULL)
			{
				fprintf(fp,"%s\t",getTokenName(nonTerm[j]->firstSet[k]->token_name));
			}
		}
		fprintf(fp,"\n");
		fprintf(fp,"Follow set:\t");
		k = 0;
		for(;k<TERMINALS;++k)
		{
			if(nonTerm[j]->followSet[k] != NULL)
			{
				fprintf(fp,"%s(Rule No: %d)\t",getTokenName(nonTerm[j]->followSet[k]->token_name),nonTerm[j]->followSet[k]->ruleNum);
			}
		}
		fprintf(fp,"\n");
		fprintf(fp,"\n");
	}

	return 0;
}
