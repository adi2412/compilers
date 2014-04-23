/* Compilers Project Group 2
// Aditya Raisinghani 2011A7PS042P
// Utkarsh Verma 2011A7PS137P
// BITS Pilani, Pilani Campus
// Second semester 2014
*/

// Contains functions for abstract syntax tree generation

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

astTree astRoot;
astTree currentASTNode;
tree currentNode;
semRuleArray semRules;

/*
// Initialises the ast structure with default values.
*/
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


/*
// Traverse to next NonTerminal Node for AST
*/
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

/*
//	Traverse to the next NonTerminal Node
*/
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

/*
// find and goto the next AST node
*/
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

/**
 * [addChildASTNodes description: adds child AST nodes to the ast rule]
 */
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
				exit(0);
			}
		}
		newNode->data = childNodes->element;
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

/**
 * [createASTTreeFromParseTree description]
 * create AST Tree from the Parse tree current node
 * recursively called for each node.
 */	
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

/**
 * [printASTTree description]
 * 	print the AST tree to terminal. Uses appropriate color formatting.
 * 	recursively called for each node.
 * @param node
 * 	the AST Tree node which is to be printed
 */
void printASTTree(astTree node)
{
	if(node == NULL)
	{
		return;
	}
	while(node!= NULL)
	{
		if(!node->element.isLeaf)
		{
			printf("\x1b[37m\x1b[1m%s \x1b[0m",getNonTermValue(node->element.nontermValue));
		}
		else
		{
			printf("\x1b[33m%s \x1b[0m",getTokenName(node->element.leafName));
			if(node->sisterNode == NULL)
				printf("\n");
		}
		if(node->childNode != NULL)
		{
			printf("---> ");
			printASTTree(node->childNode);
		}
		if(node->sisterNode != NULL)
		{
			node = node->sisterNode;
		}
		else
			node = NULL;
	}
}

/**
 * [ast description]
 * 	main function for ast module
 * 	invokes the ast structure initialiser, and the invokes the parse tree
 * 	to ast tree generator
 * @param  headRule
 * 	head node of the semrule array structure
 * @param  root
 * 	root of the parse tree
 * @return
 * 	returns the generated AST tree root.
 */
astTree ast(semRuleArray headRule, tree root)
{
	initialiseASTTree();
	currentNode = root;
	semRules = headRule;
	createASTTreeFromParseTree();
	return astRoot;
}