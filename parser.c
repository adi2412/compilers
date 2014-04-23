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

parseTable PT[NONTERMINALS][TERMINALS];
stack head;
tree currentNode;
tree root;

void error(int d)
{
	printf("An error was generated at %d\n",d);
	exit(0);
}

void initialiseParseTable()
{
	/*
	// Initialise a Parse Table.
	*/
	int i,j;
	for(i = 0; i< NONTERMINALS-1; ++i)
	{
		for(j = 0; j < TERMINALS-2; ++j)
		{
			PT[i][j].ruleNumber = 0;
		}
	}
}

void createParseTable(nonterm nonTerm)
{
	/*
	// Fill the Parse Table with Rule numbers.
	*/
	int i = 0;
	for(;i<NONTERMINALS-1;++i)
	{
		first firstSet = nonTerm[i]->firstSet;
		int j =0;
		for(;j<TERMINALS-2;++j)
		{
			if(firstSet[j] != NULL)
			{
				if(PT[i][j].ruleNumber != 0)
				{
					fprintf(stderr,"There are two clashing rules");
					exit(0);
				}
				PT[i][j].ruleNumber = firstSet[j]->ruleNum;
			}
		}
		if(nonTerm[i]->nullable)
		{
			// Contains nullable production.
			// Add follow set to parse table as well.
			follow followSet = nonTerm[i]->followSet;
			int j =0;
			for(;j<TERMINALS-2;++j)
			{
				if(followSet[j] != NULL)
				{
					PT[i][j].ruleNumber = followSet[j]->ruleNum;
				}
			}
		}
	}
}

void createStack()
{
	/*
	// Initialise the stack. Add Dollar to the bottom.
	*/
	stack stackItem = malloc(sizeof(struct _stack));
	terminal element;
	element.tokValue = DOL;
	element.nontermValue = invalidNonTerm;
	element.flag = FALSE;
	stackItem->element = element;
	stackItem->nextElement = NULL;
	head = stackItem;

	// Add start symbol above dollar.

	element.nontermValue = mainFunction;
	element.tokValue = INVALID;
	element.flag = TRUE;
	stack newItem = malloc(sizeof(struct _stack));
	newItem->element = element;
	newItem->nextElement = head;
	head = newItem;
}

void createTree()
{
	tree treeNode = malloc(sizeof(struct _tree));
	treeNode->sisterNode = NULL;
	treeNode->parentNode = NULL;
	treeNode->childNode = NULL;
	treeNode->ruleNum = 0;
	// Create root with start symbol.

	termData element;
	element.nontermValue = mainFunction;
	element.tokValue = INVALID;
	element.token_data = "";
	element.flag = TRUE;

	treeNode->element = element;
	currentNode = treeNode;
	root = treeNode;
}

/*************** Stack functions ******************/

void pop()
{
	stack stackItem = head->nextElement;
	free(head);
	head = stackItem;
}

void push(terminal el)
{
	stack newItem = malloc(sizeof(struct _stack));
	newItem->element = el;
	newItem->nextElement = head;
	head = newItem;
}

void pushItems(terminals terms)
{
	stack newTopItem = malloc(sizeof(struct _stack));
	newTopItem->element = terms->term_value;
	terms = terms->nextTerm;
	stack aboveItem = newTopItem;
	while(terms != NULL)
	{
		stack newItem = malloc(sizeof(struct _stack));
		newItem->element = terms->term_value;
		aboveItem->nextElement = newItem;
		aboveItem = newItem;
		terms = terms->nextTerm;
	}
	if(aboveItem->element.tokValue != NIL)
	{
		aboveItem->nextElement = head;
		head = newTopItem;
	}
}

/*************** End ********************/

/*************** Tree functions *******************/


void goToNextNode(tokenInfo token)
{
	if(currentNode->element.tokValue == token->token_name)
	{
		if(token->token_name == ID || token->token_name == STR || token->token_name == FUNID || token->token_name == NUM || token->token_name == RNUM)
			currentNode->element.token_data = token->token_value;
	}
	tree sister = currentNode->sisterNode;
	tree parent = currentNode->parentNode;
	while(sister == NULL)
	{
		// tree parent = currentNode->parentNode;
		if(parent == NULL)
		{
			// We reached the root element. Handle somehow.
			currentNode = NULL;
			return;
		}
		else
		{
			sister = parent->sisterNode;
			parent = parent->parentNode;
		}
	}
	if((sister->element.nontermValue != head->element.nontermValue) && (sister->element.tokValue != head->element.tokValue))
	{
		currentNode = sister;
		goToNextNode(token);
	}
	else
	{
		currentNode = sister;
	}
}

void findNextNode()
{
	tree sister = currentNode->sisterNode;
	tree parent = currentNode->parentNode;
	while(sister == NULL)
	{
		// tree parent = currentNode->parentNode;
		if(parent == NULL)
		{
			// We reached the root element. Handle somehow.
			currentNode = NULL;
			return;
		}
		else
		{
			sister = parent->sisterNode;
			parent = parent->parentNode;
		}
	}
	if((sister->element.nontermValue != head->element.nontermValue) && (sister->element.tokValue != head->element.tokValue))
	{
		currentNode = sister;
		findNextNode();
	}
	else
	{
		currentNode = sister;
	}
}

void addChildNodes(rule rule)
{
	nonTerminal LHSTerm = rule->nonterm_value;
	if(LHSTerm == currentNode->element.nontermValue)
	{
		currentNode->ruleNum = rule->ruleNum;
	}
	tree treeNode = malloc(sizeof(struct _tree));
	treeNode->parentNode = currentNode;
	treeNode->childNode = NULL;
	treeNode->sisterNode = NULL;
	treeNode->ruleNum = 0;
	terminals terms = rule->termSet;
	termData term;
	term.flag = terms->term_value.flag;
	term.nontermValue = terms->term_value.nontermValue;
	term.tokValue = terms->term_value.tokValue;
	term.token_data = "";
	treeNode->element = term;
	currentNode->childNode = treeNode;
	terms = terms->nextTerm;
	tree prevNode = treeNode;
	while(terms != NULL)
	{
		tree newNode = malloc(sizeof(struct _tree));
		newNode->parentNode = currentNode;
		prevNode->sisterNode = newNode;
		newNode->childNode = NULL;
		newNode->sisterNode = NULL;
		newNode->ruleNum = 0;
		termData term;
		term.flag = terms->term_value.flag;
		term.nontermValue = terms->term_value.nontermValue;
		term.tokValue = terms->term_value.tokValue;
		term.token_data = "";
		newNode->element = term;
		prevNode = newNode;
		terms = terms->nextTerm;
	}
	if(treeNode->element.tokValue != NIL)
	{
		currentNode = treeNode;
	}
	else
	{
		findNextNode();
	}
}

/*************** End ********************/

rule findRule(int ruleNum, rule headRule)
{
	int i = 1;
	rule ruleList;
	ruleList = headRule;
	while(i<ruleNum && ruleList != NULL)
	{
		ruleList = ruleList->nextRule;
		++i;
	}
	return ruleList;
}

void printTree(tree node)
{
	if(node == NULL)
	{
		return;
	}
	while(node!= NULL)
	{
		if(node->element.flag)
		{
			printf("\x1b[37m\x1b[1m%s \x1b[0m",getNonTermValue(node->element.nontermValue));
		}
		else
		{
			printf("\x1b[33m%s \x1b[0m",getTokenName(node->element.tokValue));
			if(node->sisterNode == NULL)
				printf("\n");
		}
		if(node->childNode != NULL)
		{
			printf("---> ");
			printTree(node->childNode);
		}
		if(node->sisterNode != NULL)
		{
			node = node->sisterNode;
		}
		else
			node = NULL;
	}	
}

tree parse(nonterm nts, tokenInfo toks, grammar hdRule)
{
	nonterm nonTerms = nts;
	tokenInfo tokens = toks;
	grammar headRule = hdRule;
	initialiseParseTable();
	createParseTable(nonTerms);
	createStack();
	createTree();
	tokenInfo readToken;
	// tokens nextToken;
	readToken = tokens;

	while(head->element.tokValue != DOL && readToken != NULL)
	{
		// if(readToken->token_value != NULL)
		// 	printf(" with value %s", readToken->token_value);
		if(!head->element.flag)
		{
			if(head->element.tokValue == readToken->token_name)
			{
				pop();
				if(readToken->token_name == currentNode->element.tokValue)
				{
					currentNode->element.lineNumber = readToken->lineNumber;
					currentNode->element.charNumber = readToken->charNumber;
				}
				goToNextNode(readToken);
				readToken = readToken->nextToken;
			}
			else
			{
				// Some other terminal in input code.
				error(1);
			}
		}
		else
		{
			// It is a non terminal.
			if(PT[head->element.nontermValue][readToken->token_name].ruleNumber == 0)
			{
				// No entry in Parse Table.
				error(2);
				
			}
			else
			{
				// Entry in the parse Table.
				int ruleNumber = PT[head->element.nontermValue][readToken->token_name].ruleNumber;
				if(ruleNumber == 0)
				{
					exit(0);
				}
				rule correctRule = findRule(ruleNumber, headRule);
				pop();
				pushItems(correctRule->termSet);
				addChildNodes(correctRule);
				// goToNextNode();
			}
		}
	}
	if(readToken == NULL)
	{
		if(head->element.tokValue == DOL)
		{
			// Successful parsing.
			return root;
		}
		else
		{
			// Parsing not successful.
			error(3);
		}
	}
	else if(head->element.tokValue == DOL)
	{
		if(readToken != NULL)
		{
			// Stack empty but input still exists.
			error(4);
		}
		else
		{
			// Succesful parsing.
			return root;
		}
	}

	return root;
}