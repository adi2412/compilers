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
	printf("Compilers group 2\n");
	printf("FIRST and FOLLOW set automated.\n");
	printf("Lexical analyzer module implmeneted.\n");
	printf("Syntax analysis-> Grammar, FIRST, FOLLOW, and Parse Table generated.\n");
	printf("Parse tree could not be constructed\n");
	printf("Press option for the defined task:\n");
	printf("1: for printing token list\n");
	int choice;
	scanf("%d",&choice);
	if(choice != 1)
	{
		printf("Invalid choice\n");
		exit(0);
	}
	else
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
		if(tokens->nextToken == NULL)
		{
			fprintf(stderr, "Kya chutiya bana raha hai\n");
		}

		FILE *rules = fopen("grammar_rules.txt","r");
		if(rules == NULL)
		{
			printf("The grammar file could not be found.\n");
			exit(1);
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
			while(readterms != NULL)
			{
				if(readterms->term_value.flag)
				{
					printf("%d\t",readterms->term_value.nontermValue);
				}
				else
					printf("%d\t",readterms->term_value.tokValue);

				readterms = readterms->nextTerm;
			}
			printf("\n");
			gramRule = gramRule->nextRule;
		}
		fclose(rules);
		nonterm nonTerms = ffg(destination);
		tree root = parse(nonTerms, tokens, headRule);
		fclose(destination);
		
		printf("Program successfully tokenized and created first and follow sets\n");

		semRuleArray  sra = returnSemanticRules();
		astTree astRoot = ast(sra, root);
		STList headList = generateSymbolTables(astRoot);
		typeChecker(astRoot, headList);
		semanticAnalyzer(astRoot, headList);
	}
	return 0;
}