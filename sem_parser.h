/* Compilers Project Group 2
// Aditya Raisinghani 2011A7PS042P
// Utkarsh Verma 2011A7PS137P
// BITS Pilani, Pilani Campus
// Second semester 2014
*/

#ifndef SEMP
#define SEMP

typedef struct _sem
{
	flag isLeaf;
	token leafName;
	char* leafValue; //ID.entry, or NUM/RNUM/STR.val
	nonTerminal nontermValue;
} sem;

typedef struct _semset *sems;

struct _semset
{
	sem sem_value;
	sems nextSem;
};

typedef struct _semrule *semrule;

typedef struct _semrule **semRuleArray;

typedef semrule semantics;

struct _semrule
{
	int ruleNum;
	choice isMakeNode_Leaf_Direct;
	nonTerminal nonterm_value; //LHS
	sems semanticsSet;	//RHS.nonterms or leaf.
	//hasEmpty nullable;	//if ===NULL
	//semrule nextRule;
	//char* nodename //maybe reqd later.AST nodename
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
	termData data;
};


semrule readAndDefineSemanticRule(FILE *fp);
char* readNonTerm(char* term, char* rule, int ptr);
sem readLeafTerm(char* rule, int ptr);
sems getRHSnodes(char* ruleString, int ptr, choice _ch);
semRuleArray returnSemanticRules();
#endif