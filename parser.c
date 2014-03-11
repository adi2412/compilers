/* Compilers Project Group 2
// Aditya Raisinghani 2011A7PS042P
// Utkarsh Verma 2011A7PS137P
// BITS Pilani, Pilani Campus
// Second semester 2014
*/

// void createParseTable(grammar G, table T);
// parseTree parseInputSourceDode(char *testcaseFile, table T);
// void printParseTree(parseTree PT, char *outfile);
// 
// finally when all NT's are populated, write: "NT; first sets; follow sets" per line for all NTs.
#include <stdio.h>
#include <stdlib.h>
#define VSIZE 30 
#define LINESIZE 1024
typedef struct _NonTerminal{
	//FIRST SET
	//FOLLOW SET
	//Productions list
	//produced in (list of NTs in which it is produced)
	
} *NTerm;
typedef char* Term;

union _NTorT{
		Term t;
		NTerm nt;
	} NTorT;



typedef struct _Production{
	NTerm LHS;
	ProductionVar pv;
	RHS 
	struct _Production *nextProduction;
} *prod;

void parseGrammar(FILE* fp)
{
	// char cur = fgetc(fp);
		// printf("NT: %c", cur);
		// char* str1;
		// char* str2;
		char *linestream= (char*)malloc(LINESIZE*sizeof(char));
		gets(linestream,LINESIZE,fp); //gets a line from file to linestream
		//fscanf(fp,"[ %s ] ===) %s %s %s [%s] %s", str1,str2,str3,str4,str5,str6);	
		char *vname=(char*)malloc(VSIZE*sizeof(char));
		fscanf(fp,"[ %s ]", vname);
		makeNT(vname)
	
		while(fp != EOF)
		{
			char *vname=(char*)malloc(VSIZE*sizeof(char));
			fscanf(fp,"[ %s ]", vname);

		}

		printf("NT %s  \n", str1);
		printf("prodcution %s %s %s %s %s \n", str2,str3,str4,str5,str6);

}


void main()
{
	FILE* fp = fopen("grammar_rules.txt","r");//must check if non-null value is returned
	
	if(fp != NULL)
	{
		parseGrammar(fp);
	}
	
}