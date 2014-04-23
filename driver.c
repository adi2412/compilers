/* Compilers Project Group 2
// Aditya Raisinghani 2011A7PS042P
// Utkarsh Verma 2011A7PS137P
// BITS Pilani, Pilani Campus
// Second semester 2014
*/


/* Main driver function */
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

int main(int argc,char* argv[])
{
	if(argc != 3)
	{
		printf("Invalid number of arguments\n");
		exit(0);
	}
	printf("\n\t\t\t\x1b[37m\x1b[1m\x1b[4mCompilers Group 2\n\n\x1b[0m");
	printf("\x1b[36m\x1b[1m\tAditya Raisinghani\t\tUtkarsh Verma\n\x1b[0m");
	printf("\x1b[37m\x1b[3m\t2011A7PS042P\t\t\t2011A7PS137P\n\n\x1b[0m");
	printf("\x1b[32m\tUnder supervision of Vandana Agarwal\n\n\x1b[0m");
	printf("\x1b[33m\x1b[1m->FIRST and FOLLOW set automated.\n");
	printf("->Lexical analyzer module implmeneted.\n");
	printf("->Syntax analysis-> Grammar, FIRST, FOLLOW, and Parse Table generated.\n");
	printf("->Semantic rules generated\n");
	printf("->Abstract syntax tree generated\n");
	printf("->Symbol table generated\n");
	printf("->Type checker, semantic analyzer implemented\n");
	printf("->Code generation could not be completed\n\x1b[0m");

	printf("\n\t\x1b[4m\x1b[40m\x1b[31m\x1b[1mLEVEL 3: Symbol table/type Checking/AST/Semantic Rules modules works\n\n\x1b[0m");
		
	printf("\n\x1b[33m\x1b[1mPress option for the defined task:\x1b[0m\n");
	printf("\x1b[37m1:\x1b[22m\x1b[1m for printing token list\x1b[0m\n");
	printf("\x1b[37m2:\x1b[22m\x1b[1m for parsing and producing Parse Tree\x1b[0m\n");
	printf("\x1b[37m3:\x1b[22m\x1b[1m for printing the Abstract Syntax Tree\x1b[0m\n");
	printf("\x1b[37m4:\x1b[22m\x1b[1m for printing Symbol Table\x1b[0m\n");
	printf("\x1b[37m5:\x1b[22m\x1b[1m for compiling to verify syntactic and semantic correctness of code\x1b[0m\n");
	printf("\x1b[37m\x1b[9m6:\x1b[22m\x1b[1m for generating code.asm (incomplete)\x1b[0m\n");
	int choice;
	scanf("%d",&choice);
	
	if(choice == 1)
	{
		char* sourceName = argv[1];
		FILE *source = fopen(sourceName,"r");
		if(source == NULL)
		{
			printf("No such file found - %s\n",sourceName);
			exit(0);
		}
		char* destName = argv[2];
		FILE *destination = fopen(destName,"w");
		if(destination == NULL)
		{
			printf("No such file found - %s\n",destName);
			exit(0);
		}
		tokenInfo headToken = lexer(source,destination);
		// Write to output file.
		tokenInfo tokens = headToken;
	}
	else if(choice == 2)
	{
		char* sourceName = argv[1];
		FILE *source = fopen(sourceName,"r");
		if(source == NULL)
		{
			printf("No such file found - %s\n",sourceName);
			exit(0);
		}
		char* destName = argv[2];
		FILE *destination = fopen(destName,"w");
		if(destination == NULL)
		{
			printf("No such file found - %s\n",destName);
			exit(0);
		}
		tokenInfo headToken = lexer(source,destination);
		// Write to output file.
		printf("\n\n");
		tokenInfo tokens = headToken;

		FILE *rules = fopen("grammar_rules.txt","r");
		if(rules == NULL)
		{
			printf("The grammar file could not be found.\n");
			exit(0);
		}

		grammar headRule;

		rule gramRule = readAndDefineGrammarRule(rules);
		int ruleNum = 1;
		// Maintain a pointer at the first rule
		headRule = gramRule;
		while(gramRule != NULL)
		{
			// Keep computing first sets and reading grammar rules
			gramRule->ruleNum = ruleNum++;
			gramRule->nextRule = readAndDefineGrammarRule(rules);
			terminals readterms = gramRule->termSet;
			gramRule = gramRule->nextRule;
		}
		fclose(rules);
		nonterm nonTerms = ffg(destination);
		tree root = parse(nonTerms, tokens, headRule);
		printf("\x1b[4m\x1b[1m\x1b[37mPrinting Parse Tree in depth first order\x1b[0m\n");
		printTree(root);
		printf("\n\n");
		fclose(destination);
	}
	else if(choice == 3)
	{
		char* sourceName = argv[1];
		FILE *source = fopen(sourceName,"r");
		if(source == NULL)
		{
			printf("No such file found - %s\n",sourceName);
			exit(0);
		}
		char* destName = argv[2];
		FILE *destination = fopen(destName,"w");
		if(destination == NULL)
		{
			printf("No such file found - %s\n",destName);
			exit(0);
		}
		tokenInfo headToken = lexer(source,destination);
		// Write to output file.
		printf("\n\n");
		tokenInfo tokens = headToken;

		FILE *rules = fopen("grammar_rules.txt","r");
		if(rules == NULL)
		{
			printf("The grammar file could not be found.\n");
			exit(0);
		}

		grammar headRule;

		rule gramRule = readAndDefineGrammarRule(rules);
		int ruleNum = 1;
		// Maintain a pointer at the first rule
		headRule = gramRule;
		while(gramRule != NULL)
		{
			// Keep computing first sets and reading grammar rules
			gramRule->ruleNum = ruleNum++;
			gramRule->nextRule = readAndDefineGrammarRule(rules);
			terminals readterms = gramRule->termSet;
			gramRule = gramRule->nextRule;
		}
		fclose(rules);
		nonterm nonTerms = ffg(destination);
		tree root = parse(nonTerms, tokens, headRule);
		fclose(destination);
		semRuleArray  sra = returnSemanticRules();
		astTree astRoot = ast(sra, root);
		printf("\x1b[4m\x1b[1m\x1b[37mPrinting AST Tree in depth first order\x1b[0m\n");
		printASTTree(astRoot);
		printf("\n\n");
	}
	else if(choice == 4)
	{
		char* sourceName = argv[1];
		FILE *source = fopen(sourceName,"r");
		if(source == NULL)
		{
			printf("No such file found - %s\n",sourceName);
			exit(0);
		}
		char* destName = argv[2];
		FILE *destination = fopen(destName,"w");
		if(destination == NULL)
		{
			printf("No such file found - %s\n",destName);
			exit(0);
		}
		tokenInfo headToken = lexer(source,destination);
		// Write to output file.
		printf("\n\n");
		tokenInfo tokens = headToken;

		FILE *rules = fopen("grammar_rules.txt","r");
		if(rules == NULL)
		{
			printf("The grammar file could not be found.\n");
			exit(0);
		}

		grammar headRule;

		rule gramRule = readAndDefineGrammarRule(rules);
		int ruleNum = 1;
		// Maintain a pointer at the first rule
		headRule = gramRule;
		while(gramRule != NULL)
		{
			// Keep computing first sets and reading grammar rules
			gramRule->ruleNum = ruleNum++;
			gramRule->nextRule = readAndDefineGrammarRule(rules);
			terminals readterms = gramRule->termSet;
			gramRule = gramRule->nextRule;
		}
		fclose(rules);
		nonterm nonTerms = ffg(destination);
		tree root = parse(nonTerms, tokens, headRule);
		fclose(destination);

		semRuleArray  sra = returnSemanticRules();
		astTree astRoot = ast(sra, root);
		STList headList = generateSymbolTables(astRoot);
		typeChecker(astRoot, headList);
		printf("\x1b[4m\x1b[1m\x1b[37mPrinting symbol table\n\x1b[0m");
		printf("\x1b[4m\x1b[36m%20s%20s%20s%20s\x1b[0m\n","Identifier name","scope","type","offset");
		printSymbolTable(headList);
		printf("\n\n");
	}
	else if(choice == 5)
	{
		char* sourceName = argv[1];
		FILE *source = fopen(sourceName,"r");
		if(source == NULL)
		{
			printf("No such file found - %s\n",sourceName);
			exit(0);
		}
		char* destName = argv[2];
		FILE *destination = fopen(destName,"w");
		if(destination == NULL)
		{
			printf("No such file found - %s\n",destName);
			exit(0);
		}
		tokenInfo headToken = lexer(source,destination);
		// Write to output file.
		tokenInfo tokens = headToken;
		printf("\n\n");
		FILE *rules = fopen("grammar_rules.txt","r");
		if(rules == NULL)
		{
			printf("The grammar file could not be found.\n");
			exit(0);
		}

		grammar headRule;

		rule gramRule = readAndDefineGrammarRule(rules);
		int ruleNum = 1;
		// Maintain a pointer at the first rule
		headRule = gramRule;
		while(gramRule != NULL)
		{
			// Keep computing first sets and reading grammar rules
			gramRule->ruleNum = ruleNum++;
			gramRule->nextRule = readAndDefineGrammarRule(rules);
			terminals readterms = gramRule->termSet;
			gramRule = gramRule->nextRule;
		}
		fclose(rules);
		nonterm nonTerms = ffg(destination);
		tree root = parse(nonTerms, tokens, headRule);
		fclose(destination);

		semRuleArray  sra = returnSemanticRules();
		astTree astRoot = ast(sra, root);
		STList headList = generateSymbolTables(astRoot);
		typeChecker(astRoot, headList);
		semanticAnalyzer(astRoot, headList);
	}
	else
	{
		printf("\x1b[31m\x1b[1mInvalid choice\n\x1b[0m");
	}
	return 0;
}