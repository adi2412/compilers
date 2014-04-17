#ifndef PARSER
#define PARSER

typedef struct _parseTable
{
	// int rowSize;
	// int colSize;
	int ruleNumber;
} parseTable;


typedef struct _stack *stack;

struct _stack
{
	terminal element;
	stack nextElement;
};

typedef struct _data
{
	terminal term_value;
} data;

typedef struct _tree *tree;

typedef struct _termData
{
	isNonTerminal flag;
	token tokValue;
	char* token_data;
	nonTerminal nontermValue;
} termData;

struct _tree
{
	tree sisterNode;
	tree parentNode;
	tree childNode; // Points to the leftmost child
	termData element;
	int ruleNum;
};

typedef sem astData;

typedef struct _astTree *astTree;

struct _astTree
{
	astTree sisterNode;
	astTree parentNode;
	astTree childNode;
	// char* name;
	int ruleNum;
	astData element;
};

int parse(nonterm nonTerms, tokenInfo tokens, grammar headRule);

#endif