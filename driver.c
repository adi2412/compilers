/* Compilers Project Group 2
// Aditya Raisinghani 2011A7PS042P
// Utkarsh Verma 2011A7PS137P
// BITS Pilani, Pilani Campus
// Second semester 2014
*/


/* Main driver function */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parserDef.h"
#include "lexerDef.h"
#include "lexer.h"
#include "first_follow_gen.h"

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

		ffg(destination);
		printf("Program successfully tokenized and created first and follow sets\n");
	}
}