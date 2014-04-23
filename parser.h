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
	int charNumber;
	int lineNumber;
} termData;

struct _tree
{
	tree sisterNode;
	tree parentNode;
	tree childNode; // Points to the leftmost child
	termData element;
	int ruleNum;
};

tree parse(nonterm nonTerms, tokenInfo tokens, grammar headRule);
void printTree(tree node);

#endif