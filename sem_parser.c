/* Compilers Project Group 2
// Aditya Raisinghani 2011A7PS042P
// Utkarsh Verma 2011A7PS137P
// BITS Pilani, Pilani Campus
// Second semester 2014
*/

//get file of sem rules
//read the file, store each NT with its semantc expansion: 
//	~ASTnodename,
//	~new leaves list (leaves are of ID, Num,rnumstr type, each has val or num style predefined)
//	~RHS: make node? 
//	~RHS: child NTs
//	
//get parsetable pointer, start traversing from top
//make AST nodesa along way is rules hold for that parseing rule
//	
//	
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

#define OFFSET2 4 //acc to semantic_rules.txt
#define OFFSET3 3 //be sem line indicator nd next term
#define NOSEMRULES 86 //no of sem rules

void printSemanticRules(semRuleArray rules)
{
	int i = 0;
	while(rules[i] != NULL)
	{
		fprintf(stderr,"Rule number %d)",i+1);
		fprintf(stderr," %s",getNonTermValue(rules[i]->nonterm_value));
		fprintf(stderr,":=> ");
		sems semSet = rules[i]->semanticsSet;
		while(semSet != NULL)
		{
			if(semSet->sem_value.isLeaf)
				fprintf(stderr,"%s(%s) ",getTokenName(semSet->sem_value.leafName),semSet->sem_value.leafValue);
			else
				fprintf(stderr,"%s ",getNonTermValue(semSet->sem_value.nontermValue));
			if(semSet->nextSem == NULL)
				break;
			else
				semSet = semSet->nextSem;
		}
		fprintf(stderr,"\n");
		++i;
	}

}


semRuleArray returnSemanticRules()
{
		FILE *sem_rules_file = fopen("semantic_rules.txt","r");
		if(sem_rules_file == NULL)
		{
			printf("The semantic rules file could not be found.\n");
			exit(1);
		}
		//semantics headRule;

		semRuleArray semRule = malloc(sizeof(semrule*)*NOSEMRULES);
		
		semRule[0] = readAndDefineSemanticRule(sem_rules_file); //gramRule=semRule

		int ruleNum = 1;
		// Maintain a pointer at the first rule
		//headRule = semRule;

		while(semRule[ruleNum-1] != NULL)
		{
			semRule[ruleNum-1]->ruleNum = ruleNum;
			ruleNum++;
		//	semRule[0]->nextRule = readAndDefineSemanticRule(sem_rules_file);
			semRule[ruleNum-1] = readAndDefineSemanticRule(sem_rules_file);
			
		}
		return semRule; //the headRule
}

semrule readAndDefineSemanticRule(FILE *fp)
{
	// Reads the file and creates a single rule.
	char ruleString[RULESIZE];
	if(fgets(ruleString,RULESIZE,fp) == NULL)
	{
		// All rules have been read.
		return NULL;
	}
	//else continue with defining.
	char* productionTerm = malloc(NONTERMSIZE*sizeof(char));
	// Read the production term and get the enumerated value
	productionTerm = readNonTerm(productionTerm, ruleString,0);
	nonTerminal nonterm_value = getNonTerminal(productionTerm); //modify the nonterm list
	semrule SRule = malloc(sizeof(struct _semrule));
	SRule->nonterm_value = nonterm_value;

	
	// Read the RHS terms.
	int pointer = 0;
	pointer = OFFSET2 + strlen(productionTerm);

	//now determine which of 3
	if(ruleString[pointer] == '#')	
		SRule->isMakeNode_Leaf_Direct = MAKENODE;
	else if(ruleString[pointer] == '^')
		SRule->isMakeNode_Leaf_Direct = LEAF;
	else if(ruleString[pointer] == '%')
		SRule->isMakeNode_Leaf_Direct = DIRECT;
	else if(ruleString[pointer] == '?')
	{
		sems RHSnodes = malloc(sizeof(struct _semset));
		RHSnodes->sem_value.isLeaf= TRUE;
		RHSnodes->sem_value.leafName = NIL;
		RHSnodes->sem_value.leafValue = "";
		RHSnodes->sem_value.nontermValue = invalidNonTerm;
		RHSnodes->nextSem = NULL;
		if(SRule->semanticsSet == NULL)
			SRule->semanticsSet = malloc(sizeof(struct _semset));
		SRule->semanticsSet = RHSnodes;
		return SRule;
	}	
	else
	{
		printf("illformed semantic rule \n");
		return NULL;
	}
	pointer += OFFSET3;

	// Compute the first non terminal
	sems RHSnodes = malloc(sizeof(struct _semset));
	RHSnodes = getRHSnodes(ruleString,pointer, SRule->isMakeNode_Leaf_Direct);
	//SRule->nullable = RHSTerm->nullable;
	if(SRule->semanticsSet == NULL)
		SRule->semanticsSet = malloc(sizeof(struct _semset));
	SRule->semanticsSet = RHSnodes;
	return SRule;
}


char* readNonTerm(char* term, char* rule, int ptr)
{
	int i = ptr;
	int j = 0;
	while(rule[i] != ']')
	{
		term[j] = rule[i];
		++i;
		++j;
	}
	term[j] = ']';
	term[j+1] = '\0';
	return term;
}

sem readLeafTerm(char* rule, int ptr)
{
	sem leafterm;
	int i = ptr;
	int j =0;
	leafterm.isLeaf = TRUE;
	leafterm.nontermValue = invalidNonTerm;
	// !UPDATE
	char* term = malloc(sizeof(char)*NONTERMSIZE); //30 is an arbit max size here
	while(rule[i] != ' ')
	{
		term[j] = rule[i];
		++i;
		++j;
	}
	leafterm.leafName = getToken(term);

  ++i;
  if(rule[i] == ')')
  {
  	leafterm.leafValue= "";
  	return leafterm;
  }
  else
	{
		int j = 0;
		char* value = malloc(sizeof(char)*NONTERMSIZE); //30 is an arbit max size here
		while(rule[i] != ' ' )
		{
		value[j] = rule[i];
		++i;
		++j;
		}
		leafterm.leafValue = value;
		//term[i] = ']';
	}	

	return leafterm;
}


sems getRHSnodes(char* ruleString, int ptr, choice _ch)
{
	sems RHSnodes = malloc(sizeof(struct _semset));
	sems headRHSnodes = RHSnodes;

	if(_ch == LEAF)
	{
		sem RHSnode = readLeafTerm(ruleString, ptr);
		int offset4 = (strcmp(RHSnode.leafValue,"") ==0)?strlen(getTokenName(RHSnode.leafName))+4:0;
		ptr += strlen(getTokenName(RHSnode.leafName)) + offset4 +2;
		RHSnodes->sem_value = RHSnode;
		RHSnodes->nextSem = NULL;

		return RHSnodes;
	}
	else if(_ch == DIRECT)
	{
		char* productionTerm = malloc(NONTERMSIZE*sizeof(char));
		// Read the production term and get the enumerated value
		productionTerm = readNonTerm(productionTerm, ruleString, ptr);
		nonTerminal nonterm_value = getNonTerminal(productionTerm); //modify the nonterm list
		ptr +=  strlen(productionTerm); //update ptr
		sem RHSnode;
		RHSnode.isLeaf = FALSE;
		RHSnode.nontermValue = nonterm_value;
		RHSnodes->sem_value = RHSnode;
		RHSnodes->nextSem = NULL;
		return RHSnodes;
	}
	else //_ch == MAKENODE
	{
		sems prevNode;
		while(ruleString[ptr] != ')')
		{
			if(ruleString[ptr] == '^')
			{
				//ensure close paren is already past when ptr returns from readleaf
				ptr += OFFSET3;
				sem RHSnode = readLeafTerm(ruleString, ptr);
				int offset4 = (strcmp(RHSnode.leafValue,"") ==0)?0:strlen(getTokenName(RHSnode.leafName))+5;
				ptr += strlen(getTokenName(RHSnode.leafName)) + offset4 +2;
				if(RHSnodes == NULL)
				{
					RHSnodes = malloc(sizeof(struct _semset));
				}
				RHSnodes->sem_value = RHSnode;
				sems nextNode = malloc(sizeof(struct _semset));
				RHSnodes->nextSem = nextNode;
				prevNode = RHSnodes;
				RHSnodes = nextNode;
			}		
			else if(ruleString[ptr] == '[')
			{
				char* productionTerm = malloc(NONTERMSIZE*sizeof(char));
				// Read the production term and get the enumerated value
				productionTerm = readNonTerm(productionTerm, ruleString, ptr);
				nonTerminal nonterm_value = getNonTerminal(productionTerm); //modify the nonterm list
				ptr +=  strlen(productionTerm); //update ptr
				sem RHSnode;
				RHSnode.isLeaf = FALSE;
				RHSnode.leafName = INVALID;
				RHSnode.leafValue = "";
				RHSnode.nontermValue = nonterm_value;
				RHSnodes->sem_value = RHSnode;
				sems nextNode = malloc(sizeof(struct _semset));
				RHSnodes->nextSem = nextNode;
				prevNode = RHSnodes;
				RHSnodes = nextNode;
			}
			else 
			{
				ptr++;
			}

		}
		if(RHSnodes->sem_value.nontermValue == 0)
		{
			free(RHSnodes);
			if(prevNode != NULL)
				prevNode->nextSem = NULL;
		}
	}
	return headRHSnodes;
}



