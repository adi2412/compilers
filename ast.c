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
			goToNextNonTerminalASTNode();
		}
	}
}

void goToNextNonTerminalNode()
{
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
			goToNextNonTerminalNode();
		}
	}
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
		astTree newNode = malloc(sizeof(struct _astTree));
		newNode->parentNode = currentASTNode;
		treeNode->childNode = NULL;
		treeNode->sisterNode = NULL;
		treeNode->ruleNum = 0;
		sem semTerm = semTerms->sem_value;
		treeNode->element = semTerm;
		prevNode->sisterNode = newNode;
		prevNode = newNode;
		semTerms = semTerms->nextSem;
	}
	currentASTNode = currentASTNode->childNode;
	if(currentASTNode->element.isLeaf)
		goToNextNonTerminalASTNode();
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
				addChildASTNodes(semRule);
				currentNode = currentNode->childNode;
				if(!currentNode->element.flag)
					goToNextNonTerminalNode();
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
				int ruleNum = currentNode->ruleNum;
				semrule semRule = semRules[ruleNum-1];
				currentASTNode->ruleNum = ruleNum;
				addChildASTNodes(semRule);
				currentNode = currentNode->childNode;
				if(!currentNode->element.flag)
					goToNextNonTerminalNode();
				createASTTreeFromParseTree();
			}
		}
	}
}

int main()
{
	initialiseASTTree();
	currentNode = root;
	semRules = headRule;
	createASTTreeFromParseTree();
}