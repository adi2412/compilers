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
	if(currentASTNode->childNode != NULL)
	{
		currentASTNode = currentASTNode->childNode;
	}
	else
	{
		// It does not have any children.
		if(currentASTNode->sisterNode != NULL)
		{
			currentASTNode = currentASTNode->sisterNode;
			return;
		}
		else
		{
			if(currentASTNode->parentNode == NULL)
			{
				currentASTNode = NULL;
				return;
			}
			else
			{
				currentASTNode = currentASTNode->parentNode;
				while(currentASTNode->sisterNode == NULL)
				{
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
		if(currentNode->sisterNode != NULL)
		{
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
	tree childNodes = currentNode->childNode;
	astTree treeNode = malloc(sizeof(struct _astTree));
	treeNode->parentNode = currentASTNode;
	treeNode->childNode = NULL;
	treeNode->sisterNode = NULL;
	treeNode->ruleNum = 0;
	sem semTerm = semTerms->sem_value;
	treeNode->element = semTerm;
	while((semTerm.isLeaf && (childNodes->element.tokValue != semTerm.leafName)) || (!semTerm.isLeaf && (childNodes->element.nontermValue != semTerm.nontermValue)))
	{
		childNodes = childNodes->sisterNode;
		if(childNodes == NULL)
		{
			fprintf(stderr,"It got to NULL\n");
			exit(0);
		}
	}
	treeNode->data = childNodes->element;
	// childNodes = childNodes->sisterNode;
	currentASTNode->childNode = treeNode;
	semTerms = semTerms->nextSem;
	astTree prevNode = treeNode;
	while(semTerms != NULL)
	{
		astTree newNode = malloc(sizeof(struct _astTree));
		newNode->parentNode = currentASTNode;
		newNode->childNode = NULL;
		newNode->sisterNode = NULL;
		newNode->ruleNum = 0;
		sem semTerm = semTerms->sem_value;
		newNode->element = semTerm;
		while((semTerm.isLeaf && (childNodes->element.tokValue != semTerm.leafName)) || (!semTerm.isLeaf && (childNodes->element.nontermValue != semTerm.nontermValue)))
		{
			childNodes = childNodes->sisterNode;
			if(childNodes == NULL)
			{
				fprintf(stderr,"It got to NULL\n");
				exit(0);
			}
		}
		printf("%s == %s\n",getTokenName(semTerm.leafName),getTokenName(childNodes->element.tokValue));
		newNode->data = childNodes->element;
		printf("Adding data %s for %s\n",treeNode->data.token_data,getTokenName(newNode->data.tokValue));
		// childNodes = childNodes->sisterNode;
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
	while(currentASTNode->element.isLeaf)
	{
		goToNextNonTerminalASTNode();
		if(currentASTNode == NULL)
		{
			return;
		}
	}
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
				currentASTNode->data = currentNode->element;
				int ruleNum = currentNode->ruleNum;
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
			else
			{
				while(currentNode->element.nontermValue != currentASTNode->element.nontermValue)
				{
					goToNextNonTerminalNode();
					if(currentNode == NULL)
						return;
				}
				currentASTNode->data = currentNode->element;
				int ruleNum = currentNode->ruleNum;
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
			printf("Node: %s(%d)(%s)\n",getNonTermValue(node->element.nontermValue),node->ruleNum,node->data.token_data);
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
			printf("Leaf node: %s(%s)", getTokenName(node->element.leafName),node->data.token_data);
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

astTree ast(semRuleArray headRule, tree root)
{
	initialiseASTTree();
	currentNode = root;
	semRules = headRule;
	createASTTreeFromParseTree();
	printASTTree(astRoot);
	return astRoot;
}