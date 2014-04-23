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

#define ERRLINESIZE 200
parseTable PT[NONTERMINALS][TERMINALS];
stack head;
tree currentNode;
tree root;

void showPError(char *errtext,int errline)
{
	printf("\n\x1b[31mParsing error.\x1b[0m");
	if ( errline != -1)
	printf("\n\x1b[31m%sAt line %d\n\x1b[0m", errtext,errline);
	else
	printf("\n\x1b[31m%s\n\x1b[0m", errtext);
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
					//fprintf(stderr,"There are two clashing rules");
					//exit(0);
					char *err =	malloc(sizeof(char)*ERRLINESIZE);
					sprintf(err,"There are two clasing rules.");
					showPError(err,-1);
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
	printf("Top of stack now has %d(%d)",head->element.tokValue,head->element.nontermValue);
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
		printf("Top of stack now has %d(%d)",head->element.tokValue,head->element.nontermValue);
	}
}

/*************** End ********************/

/*************** Tree functions *******************/


void goToNextNode(tokenInfo token)
{
	if(currentNode->element.tokValue == token->token_name)
	{
		printf("Add data to the node\n");
		if(token->token_name == ID || token->token_name == STR || token->token_name == FUNID)
			currentNode->element.token_data = token->token_value;
	}
	else
	{
		printf("Data not being added\n");
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
		fprintf(stderr, "Hello there!\n");
	}
	printf("New current node from sister is %d",sister->element.nontermValue);
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
		fprintf(stderr, "Hey there!\n");
	}
	printf("New current node from sister is %d",sister->element.nontermValue);
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
		printf("adding rule number to parse tree %d\n",rule->ruleNum);
		currentNode->ruleNum = rule->ruleNum;
	}
	else
		printf("not adding rule numbers\n");
	printf("before adding children, current node is: %d(%d)",currentNode->element.tokValue,currentNode->element.nontermValue);
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
		printf("Adding sister node %d(%d)\n",newNode->element.tokValue,newNode->element.nontermValue);
		prevNode = newNode;
		terms = terms->nextTerm;
	}
	if(treeNode->element.tokValue != NIL)
	{
		currentNode = treeNode;
		printf("current node: %d(%d)",currentNode->element.tokValue,currentNode->element.nontermValue);
	}
	else
	{
		findNextNode();
	}
}

/*************** End ********************/

rule findRule(int ruleNum, rule headRule)
{
	fprintf(stderr, "Here!!!!\n");
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

int printTree(tree node)
{
	if(node == NULL)
	{
		fprintf(stderr, "It is null\n");
		return 0;
	}
	else
	{
		fprintf(stderr,"Starting to print\n");
		if(node->element.flag)
		{
			fprintf(stderr,"Node: %s(%d)\n",getNonTermValue(node->element.nontermValue),node->ruleNum);
			tree childNode = node->childNode;
			if(childNode == NULL)
			{
				fprintf(stderr,"No child nodes\n");
				if(node->sisterNode != NULL)
					printTree(node->sisterNode);
				else
				{
					tree parentSisterNode = node->parentNode->sisterNode;
					while(parentSisterNode == NULL)
					{
						node = node->parentNode;
						if(node == NULL)
							return 0;
						else
							parentSisterNode = node->parentNode->sisterNode;
					}
					printTree(parentSisterNode);
				}
			}
			else
			{
				fprintf(stderr,"child nodes\n");
				printTree(childNode);
			}
		}
		else
		{
			// Terminal.
			fprintf(stderr,"Leaf Node: %s", getTokenName(node->element.tokValue));
			if(node->element.tokValue == ID || node->element.tokValue == STR || node->element.tokValue == FUNID)
				fprintf(stderr,"(%s)",node->element.token_data);
			fprintf(stderr,"\n");
			if(node->sisterNode != NULL)
			{
				printTree(node->sisterNode);
			}
			else
			{
				if(node->parentNode == NULL)
					return 0;
				else
				{
					while(node->parentNode->sisterNode == NULL)
					{
						if(node->parentNode == NULL)
							return 0;
						else
						{
							node = node->parentNode;
							if(node->parentNode == NULL)
								return 0;
						}
					}
					tree parentSisterNode = node->parentNode->sisterNode;
					if(parentSisterNode != NULL)
						printTree(parentSisterNode);
					else
						return 0;
				}
			}
		}
	}
	return 0;
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
	printf("%d\n",readToken->token_name);

	while(head->element.tokValue != DOL && readToken != NULL)
	{
		printf("Working on token %d", readToken->token_name);
		// if(readToken->token_value != NULL)
		// 	printf(" with value %s", readToken->token_value);
		printf("\n\n\n");
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
				printf("Popping %d\n", head->element.tokValue);
				goToNextNode(readToken);
				readToken = readToken->nextToken;
			}
			else
			{
				// Some other terminal in input code.
				char *err = malloc(sizeof(char)*ERRLINESIZE);
				if(head->element.flag)		//nonterminal
				sprintf(err,"Wrong token. Expected %s instead of %s.\n",getNonTermValue(head->element.nontermValue),getTokenName(readToken->token_name));
				else
				sprintf(err,"Wrong token. Expected %s instead of %s.\n",getTokenName(head->element.tokValue),getTokenName(readToken->token_name));
				showPError(err,readToken->lineNumber);
				///err soln
				//goToNextNode(readToken);
				//readToken = readToken->nextToken;

			}
		}
		else
		{
			// It is a non terminal.
			if(PT[head->element.nontermValue][readToken->token_name].ruleNumber == 0)
			{
				// No entry in Parse Table.
				char *err = malloc(sizeof(char)*ERRLINESIZE);
				sprintf(err,"Entry not found in parse table for %s and %s.",getNonTermValue(head->element.nontermValue),getTokenName(readToken->token_name));
				showPError(err,readToken->lineNumber);
				
			}
			else
			{
				// Entry in the parse Table.
				int ruleNumber = PT[head->element.nontermValue][readToken->token_name].ruleNumber;
				if(ruleNumber == 0)
				{
					char *err = malloc(sizeof(char)*ERRLINESIZE);
					sprintf(err,"Entry not found in parse table for %s and %s.", getNonTermValue(head->element.nontermValue),getTokenName(readToken->token_name));
					showPError(err, readToken->lineNumber);
					//exit(0);
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
			printTree(root);
		}
		else
		{
			// Parsing not successful.
			char *err =	malloc(sizeof(char)*ERRLINESIZE);
			sprintf(err,"Parsing not successful. token: %s, nonterm: %s,", getTokenName(head->element.tokValue),getNonTermValue(head->element.nontermValue));
			//printf("%d\n",head->element.tokValue);
			//printf("%d\n",head->element.nontermValue);
			showPError(err,-1);
		}
	}
	else if(head->element.tokValue == DOL)
	{
		if(readToken != NULL)
		{
			// Stack empty but input still exists.
			char *err =	malloc(sizeof(char)*ERRLINESIZE);
			sprintf(err,"Stack empty, but input still exists. token: %s.",getTokenName(readToken->token_name));
			//printf("%d\n",readToken->token_name);
			showPError(err,readToken->lineNumber);
		}
		else
		{
			// Succesful parsing.
			printTree(root);
		}
	}

	return root;
}