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
#define LINESIZE 512
// typedef struct _NonTerminal{
// 	//FIRST SET
// 	//FOLLOW SET
// 	//Productions list
// 	//produced in (list of NTs in which it is produced)
	
// } *NTerm;
// typedef char* Term;

// union _NTorT{
// 		Term t;
// 		NTerm nt;
// 	} NTorT;

typedef strcut _ProductionVar
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
	// char cur = fgetc(fp);
		// printf("NT: %c", cur);
		// char* str1;
		// char* str2;
		// 
		int grammarLine = 0;

		// char *linestream= (char*)malloc(LINESIZE*sizeof(char));
		// gets(linestream,LINESIZE,fp); //gets a line from file to linestream
		//fscanf(fp,"[ %s ] ===) %s %s %s [%s] %s", str1,str2,str3,str4,str5,str6);	
		// char *vname=(char*)malloc(VSIZE*sizeof(char));
		// fscanf(fp,"[ %s ]", vname);
		// makeNT(vname)
		

		int PT[/*no of non terms*/][/*no of terminals*/];
		int PT[5][6]; //hardcoded for now
	
		while(fGRAMMAR != EOF)
		{
	//		char *vname=(char*)malloc(VSIZE*sizeof(char));
	//		fscanf(fp,"[ %s ]", vname);
			char *linestream= (char*)malloc(LINESIZE*sizeof(char));
			fgets(linestream,LINESIZE,fGRAMMAR); //gets a line from file to linestream
			char *NT = (char*)malloc(sizeof(char)*VSIZE);
			char *prod = (char*)malloc(sizeof(char)*VSIZE);
			char *firstSet = (char*)malloc(sizeof(char)*LINESIZE);
			sscanf(linestream, "%s ; %s", NT, prod ); //string scanf in format
			//find FIRST (prod), add this rule to each e in the set, to PT[NT,e]
			firstSet = getFSTFLW(prod, fFIRST);
			while(firstSet != EOF){
				//char *e = !MODIFY! update this
			}

		}

		printf("NT %s  \n", str1);
		printf("prodcution %s %s %s %s %s \n", str2,str3,str4,str5,str6);

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
	char* set= (char*)calloc(LINESIZE*sizeof(char));
	int flag=0; //turns 1 after semicolon found
	int j=0;
	for(int i=0;i<strlen(line);i++)
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