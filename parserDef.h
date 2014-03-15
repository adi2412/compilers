/* Compilers Project Group 2
// Aditya Raisinghani 2011A7PS042P
// Utkarsh Verma 2011A7PS137P
// BITS Pilani, Pilani Campus
// Second semester 2014
*/

//Contains all definitions for data types such as grammar, table, parseTree etc. used in parser.c
#ifndef PARSERDEF
#define PARSERDEF
#include "tokens.h"

#define RULESIZE 150
#define NONTERMSIZE 25
#define TERMINALS 44 // Number of terminals that exist
#define NONTERMINALS 40 // Number of non terminals that exist
#define OFFSET 6 // defined by the format adopted in grammar_rules.txt

/*
// An enumeration of non Terminals.
*/

typedef enum nonTermValue
{
	mainFunction,
	stmtsAndFunctionDefs,
	stmtOrFunctionDef,
	stmt,
	functionDef,
	parameter_list,
	type,
	remainingList,
	declarationStmt,
	var_list,
	more_ids,
	assignmentStmt_type1,
	assignmentStmt_type2,
	leftHandSide_singleVar,
	leftHandSide_listVar,
	rightHandSide_type1,
	rightHandSide_type2,
	sizeExpression,
	ifStmt,
	otherStmts,
	ioStmt,
	funCallStmt,
	inputParameterList,
	listVar,
	arithmeticExpression,
	arithmeticTerm,
	factor,
	operator_lowPrecedence,
	operator_highPrecedence,
	booleanExpression,
	constrainedVars,
	var,
	matrix,
	rows,
	row,
	remainingColElements,
	matrixElement,
	logicalOp,
	relationalOp,
	invalidNonTerm
} nonTerminal;

typedef enum isNonTerm
{
	FALSE, TRUE
} isNonTerminal;

typedef enum isNonTerm hasEmpty;

typedef enum isNonTerm isNullable;


/*
// Structure for storing first and follow sets of different non Terminals.
*/

typedef struct _first **first;
typedef struct _follow **follow;

struct _first
{
	token token_name;
	int ruleNum;
};

struct _follow
{
	token token_name;
	int ruleNum;
};

typedef struct _nonterm **nonterm;

typedef struct _nonterm *NT;

struct _nonterm
{
	nonTerminal nonterm_value;
	first firstSet;
	follow followSet;
	isNullable nullable;
};


/*
// Structures for the RHS terms
// terminals: pointer to a structure for a term
// struct _termSet: a structure for the RHS terms used to form a linked list of such terms.
// terminal: a structure for a single RHS term, containing the token/nonTerminal value.
*/

typedef struct _terminal
{
	isNonTerminal flag;
	token tokValue;
	nonTerminal nontermValue;
} terminal;

typedef struct _termSet *terminals;

struct _termSet
{
	terminal term_value;
	terminals nextTerm;
	hasEmpty nullable;
};


/*
// The structure for the grammar rule.
// nonTerminal: Enumeration of non terminals(LHS of the rule).
// terminals: linked list of the RHS of the rule(Name is misrepresenting- it could be a non terminal as well)
// rule: Points to the next grammar Rule.
*/

typedef struct _rule *rule;

typedef rule grammar;

struct _rule
{
	int ruleNum;
	nonTerminal nonterm_value;
	terminals termSet;
	hasEmpty nullable;
	rule nextRule;
};

#endif