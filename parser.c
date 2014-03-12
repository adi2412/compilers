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
// need to define enum for NT and T
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define VSIZE 30 
#define LINESIZE 512

typedef enum nonterminals {
	E,
	E',
	T,
	T',
	F
} nontermsymbol;

typedef enum terminals {
	+,
	*,
	$,
	(,
	),
	ID
} termsymbol;

typedef struct _ProductionVar
{
	char* pvar_name;
	int pvar_type;
	PVar* next;
} *PVar;

typedef struct _ProductionRule
{
	PV LHS;
	PV* RHS; //head of list
} *PRule;

void parseGrammar(FILE* fGRAMMAR, FILE* fFOLLOW, FILE* fFIRST)
{
		int grammarLine = 0;

		//int PT[/*no of non terms*/][/*no of terminals*/];
		int PT[5][6]; //hardcoded for now
	
		while(fGRAMMAR != EOF)
		{
			char *linestream= (char*)malloc(LINESIZE*sizeof(char));
			if(fgets(linestream,LINESIZE,fGRAMMAR) == NULL) //gets a line from file to linestream
				break;
			grammarLine++;
			char *NT = (char*)malloc(sizeof(char)*VSIZE);
			char *prod = (char*)malloc(sizeof(char)*VSIZE);
			// char *firstSet = (char*)malloc(sizeof(char)*LINESIZE);
			char firstSet[LINESIZE];
			sscanf(linestream, "%s ; %s", NT, prod ); //string scanf in format
			//find FIRST (prod), add this rule to each e in the set, to PT[NT,e]
			firstSet = getFSTFLW(prod, fFIRST); //hope this works.
			// char search_string[]="Some words here for example";
  			char *array[50];
  			int emptyTransfound = 0;
  			int dollarfound = 0;
  			if(getWords(firstSet,array) == 1) //success in getting words
  			{
			  	int j;
				for(j=0;j<50;j++)
			   {
			      if(array[j]==NULL) 
			      break; //come out when array ends
			      //op
			      PT[NT][array[j]] = grammarLine; //production no
			      if(array[j] == '?')
			      	{ emptyTransfound = 1;}
				}
  			}

  			if(emptyTransfound)
  			{	
  				char followSet[LINESIZE];
  				followSet = getFSTFLW(prod, fFOLLOW); //hope this works.
  				char *array[50];
  				if(getWords(followSet,array) == 1) //success in getting words
	  			{
				  	int j;
					for(j=0;j<50;j++)
				   {
				      if(array[j]==NULL) 
				      break; //come out when array ends
				      //op
				      PT[NT][array[j]] = grammarLine; //production no
				      if(array[j] == '$')
				      	{ dollarfound = 1;}
					}
	  			}
  			}

  			if(emptyTransfound && dollarfound)
  			{
  				PT[NT][$] = grammarLine; //production no
  			}

		}
}


char* getFSTFLW(char* Term, FILE* ff)
{
	char *linestream= (char*)malloc(LINESIZE*sizeof(char));
	char *NT = (char*)malloc(sizeof(char)*VSIZE);
	do
	{	

		fgets(linestream,LINESIZE,ff); //gets a line from file to linestream
		if(sscanf(linestream, "%s", NT))  	//string scanf in format
		{ 
			if(strcmp(NT,Term) == 0) //0:matches
			{	
				//return whole string after semicolon till lineend
				return getSet(linestream);
			}
			//if not found, keep looping
		}
	} while (ff != EOF);
	//before returning remember to return fseek for ff to original position
	return NULL; //not found a FIRST set for Term
}


char* getSet(char* line)
{	
	//returns set after semicolon till lineend
	char* set= (char*)calloc(LINESIZE,sizeof(char));
	int flag=0; //turns 1 after semicolon found
	int i,j=0;
	for(i=0;i<strlen(line);i++)
	{
		if(flag)
		{
			set[j++] = line[i];
		}
		if(line[i]==';')
		{flag=1;}
	}
	return set;
}

int getWords(char a[], char *array[])
{  
  int i;
  //char *array[50];
  array[0]=strtok(a," ");
  if(array[0]==NULL)
   {
     return 0; //fail
   }

  for(i=1;i<50;i++)
   {
    array[i]=strtok(NULL," ");
    if(array[i]==NULL)
      break;
   }
  return 1; //success
}


void main()
{
	FILE* fGRAMMAR = fopen("DUMMY_GRAMMAR.txt","r");//must check if non-null value is returned
	FILE* fFIRST = fopen("DUMMY_FIRST.txt","r");//must check if non-null value is returned
	FILE* fFOLLOW = fopen("DUMMY_FOLLOW.txt","r");//must check if non-null value is returned

	
	if(fGRAMMAR == NULL || fFOLLOW == NULL || fFIRST == NULL)
	{
		printf("error loading files\n");
	}
	else
	{
		parseGrammar(fGRAMMAR, fFOLLOW,fFIRST); //should return ParseTable: int[][]
	}
	
}