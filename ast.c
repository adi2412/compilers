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

astTree astRoot;
astTree currentASTNode;
tree currentNode;
semRuleArray semRules;

void initialiseASTTree()
{
	astTree treeNode = malloc(sizeof(struct _astTree));
	treeNode->sisterNode = NULL;
	treeNode->parentNode = NULL;
	treeNode->childNode = NULL;
	treeNode->ruleNum = 0;
	// Create root with start symbol.

	astData element;
	element.nontermValue = mainFunction;
	element.leafName = INVALID;
	element.leafValue = "";
	element.isLeaf = FALSE;

	treeNode->element = element;
	currentASTNode = treeNode;
	astRoot = treeNode;
}

void goToNextNonTerminalASTNode()
{
	fprintf(stderr,"Oh yea\n");
	if(currentASTNode->childNode != NULL)
	{
		currentASTNode = currentASTNode->childNode;
	}
	else
	{
		// It does not have any children.
		fprintf(stderr, "No children\n");
		if(currentASTNode->sisterNode != NULL)
		{
			currentASTNode = currentASTNode->sisterNode;
			return;
		}
		else
		{
			fprintf(stderr, "No sister\n");
			if(currentASTNode->parentNode == NULL)
			{
				currentASTNode = NULL;
				return;
			}
			else
			{
				fprintf(stderr, "Baap ke paas jana padha\n");
				currentASTNode = currentASTNode->parentNode;
				while(currentASTNode->sisterNode == NULL)
				{
					fprintf(stderr, "Baap ki behen nahi hai\n");
					if(currentASTNode->parentNode == NULL)
					{
						currentASTNode = NULL;
						return;
					}
					else
						currentASTNode = currentASTNode->parentNode;
				}
				currentASTNode = currentASTNode->sisterNode;
			}
		}
	}
}

void goToNextNonTerminalNode()
{
	if(currentNode->childNode != NULL)
	{
		currentNode = currentNode->childNode;
	}
	else
	{
		// It does not have any children.
		fprintf(stderr, "No child\n");
		if(currentNode->sisterNode != NULL)
		{
			fprintf(stderr, "Meri behen\n");
			currentNode = currentNode->sisterNode;
			return;
		}
		else
		{
			if(currentNode->parentNode == NULL)
			{
				currentNode = NULL;
				return;
			}
			else
			{
				currentNode = currentNode->parentNode;
				while(currentNode->sisterNode == NULL)
				{
					if(currentNode->parentNode == NULL)
					{
						currentNode = NULL;
						return;
					}
					else
						currentNode = currentNode->parentNode;
				}
				currentNode = currentNode->sisterNode;
			}
		}
	}
}

void findNextASTNode()
{
	astTree sister = currentASTNode->sisterNode;
	astTree parent = currentASTNode->parentNode;
	while(sister == NULL)
	{
		if(parent == NULL)
		{
			currentASTNode = NULL;
			return;
		}
		else
		{
			sister = parent->sisterNode;
			parent = parent->parentNode;
		}
	}
	currentASTNode = sister;
}


void addChildASTNodes(semrule rule)
{
	sems semTerms = rule->semanticsSet;
	astTree treeNode = malloc(sizeof(struct _astTree));
	treeNode->parentNode = currentASTNode;
	treeNode->childNode = NULL;
	treeNode->sisterNode = NULL;
	treeNode->ruleNum = 0;
	sem semTerm = semTerms->sem_value;
	treeNode->element = semTerm;
	currentASTNode->childNode = treeNode;
	semTerms = semTerms->nextSem;
	astTree prevNode = treeNode;
	while(semTerms != NULL)
	{
		fprintf(stderr,"Going over here\n");
		astTree newNode = malloc(sizeof(struct _astTree));
		newNode->parentNode = currentASTNode;
		newNode->childNode = NULL;
		newNode->sisterNode = NULL;
		newNode->ruleNum = 0;
		sem semTerm = semTerms->sem_value;
		newNode->element = semTerm;
		prevNode->sisterNode = newNode;
		prevNode = newNode;
		semTerms = semTerms->nextSem;
	}
	if(currentASTNode->childNode != NULL)
	{
		currentASTNode = currentASTNode->childNode;
	}
	else
	{
		goToNextNonTerminalASTNode();
	}
	fprintf(stderr,"Reach here\n");
	fprintf(stderr, "%d\n", currentASTNode->element.isLeaf);
	while(currentASTNode->element.isLeaf)
	{
		fprintf(stderr,"Going into while");
		goToNextNonTerminalASTNode();
		if(currentASTNode == NULL)
		{
			return;
		}
	}
	fprintf(stderr,"Ok bye\n");
}

void createASTTreeFromParseTree()
{
	if(currentNode == NULL)
	{
		return;
	}
	else
	{
		if(currentNode->element.flag)
		{
			// Non terminal in current node.
			if(currentNode->element.nontermValue == currentASTNode->element.nontermValue)
			{
				int ruleNum = currentNode->ruleNum;
				semrule semRule = semRules[ruleNum-1];
				currentASTNode->ruleNum = ruleNum;
				printf("Adding child nodes for %s(%d)\n",getNonTermValue(semRule->nonterm_value),ruleNum);
				addChildASTNodes(semRule);
				currentNode = currentNode->childNode;
				while(!currentNode->element.flag)
				{
					goToNextNonTerminalNode();
					if(currentNode == NULL)
						return;
				}
				fprintf(stderr,"I'm here\n");
				createASTTreeFromParseTree();
			}
			else
			{
				fprintf(stderr,"Over here\n");
				while(currentNode->element.nontermValue != currentASTNode->element.nontermValue)
				{
					fprintf(stderr, "Trying to find another node because %d and %d\n",currentNode->element.nontermValue,currentASTNode->element.nontermValue);
					goToNextNonTerminalNode();
					if(currentNode == NULL)
						return;
				}
				fprintf(stderr,"rule number %d\n",currentNode->ruleNum);
				int ruleNum = currentNode->ruleNum;
				fprintf(stderr,"Found %d",ruleNum);
				semrule semRule = semRules[ruleNum-1];
				currentASTNode->ruleNum = ruleNum;
				addChildASTNodes(semRule);
				currentNode = currentNode->childNode;
				while(!currentNode->element.flag)
				{
					goToNextNonTerminalNode();
					if(currentNode == NULL)
						return;
				}
				createASTTreeFromParseTree();
			}
		}
	}
}

void printASTTree(astTree node)
{
	if(node == NULL)
	{
		return;
	}
	else
	{
		if(!node->element.isLeaf)
		{
			printf("Node: %s(%d)\n",getNonTermValue(node->element.nontermValue),node->ruleNum);
			astTree childNode = node->childNode;
			if(childNode == NULL)
			{
				if(node->sisterNode != NULL)
					printASTTree(node->sisterNode);
				else
				{	
					if(node->parentNode == NULL)
					return;
					else
					{
						while(node->parentNode->sisterNode == NULL)
						{
							if(node->parentNode == NULL)
								return;
							else
							{
								node = node->parentNode;
								if(node->parentNode == NULL)
									return;
							}
						}
						astTree parentSisterNode = node->parentNode->sisterNode;
						if(parentSisterNode != NULL)
							printASTTree(parentSisterNode);
						else
							return;
					}
				}
			}
			else
			{
				printASTTree(childNode);
			}
		}
		else
		{
			printf("Leaf node: %s", getTokenName(node->element.leafName));
			if(node->element.leafName== ID || node->element.leafName == STR || node->element.leafName == FUNID)
				printf("(%s)",node->element.leafValue);
			printf("\n");
			if(node->sisterNode != NULL)
			{
				printASTTree(node->sisterNode);
			}
			else
			{
				if(node->parentNode == NULL)
					return;
				else
				{
					while(node->parentNode->sisterNode == NULL)
					{
						if(node->parentNode == NULL)
							return;
						else
						{
							node = node->parentNode;
							if(node->parentNode == NULL)
								return;
						}
					}
					astTree parentSisterNode = node->parentNode->sisterNode;
					if(parentSisterNode != NULL)
						printASTTree(parentSisterNode);
					else
						return;
				}
			}
		}
	}
	return;
}

int ast(semRuleArray headRule, tree root)
{
	initialiseASTTree();
	currentNode = root;
	semRules = headRule;
	createASTTreeFromParseTree();
	printASTTree(astRoot);
	return 0;
}