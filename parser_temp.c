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
#include "parserDef.h"
#include "lexer.h"
#define VSIZE 30 
#define LINESIZE 512
#define EMPTY -1


typedef struct _parseTable{	
	int rowSize;
	int colSize;
	int **array; 
} ParseTable;

//////////////////////TREE FOLLOWS//////////////////
struct treenode; 
typedef struct treenode TN; 
// typedef TN* children;
struct treenode
{	
	flag isLeaf;
	tokenInfo token;		//the data contained is a token
	*nonterm nonTerminal;
	TN* sisternode;
	TN* parent;
	TN* childNode; //leftmost node
	
};

typedef struct {
	TN* root;
	TN* currNode; //currentNode

}treeroot;

treeroot* createTree()
{
	treeroot* _tr = malloc(sizeof(treeroot));
	_tr->root = NULL;
	_tr->currNode = NULL;
	return _tr;
}

treeroot* expandNode(treeroot* tr,token t, ParseTable T,strcut _nonterm **nonterm,) //terminals list, and token_name passed
{	//currNode expansion acc to rule in t's column
	int grammarLine = T[tr->currNode->currToken]
	pushExpansion(treeroot *tr,)
	//UPDATE 

}

treeroot* addNode(tokenInfo tk,treeroot* tr)
{
	TN* tnode = malloc(sizeof(TN));
	tnode->currToken = tk;
	tr->currNode->currToken = tk;
	tr->currNode->chnodes = NULL;

}
//////////////////////TREE ADT ENDS/////////////////


//////////////////STACK FOLLOWS///////////////////////

typedef struct stackLayer SL;
struct stackLayer{
	terminal stackSymbol;
	SL* below;
};

typedef struct{
	int size;//current size of stack
	SL* top;
	SL* bottom;
}stackHead;

stackHead* createStack()
{
	stackHead* sh = malloc(sizeof(stackHead));
	sh->top = NULL;
	sh->bottom = NULL;
	sh->size = 0;
	return sh;
}

int isEmpty(stackHead* shead)
{
	if(shead->bottom == NULL)
		return 1; // yes, is empty
	else
		return 0; //non empty
}

terminal topStack(stackHead* shead)
{
	return shead->top->stackSymbol;
}

stackHead* push(terminal t, stackHead* shead )
{
	SL* layer = malloc(sizeof(SL));
	layer->stackSymbol = t;
	if(shead->top == NULL){
		layer->below = NULL;
		shead->top = layer;
		shead->bottom = layer;
	}
	else{
		layer->below = shead->top;
		shead->top = layer;
	}
	sh->size++;
	return shead;
}

terminal pop(stackHead* shead)
{
	SL* del = shead->top;
	shead->top = shead->top->below;
	shead->size--;
	return del->stackSymbol;
}

stackHead* matchStack(stackHead* shead, token tk,rule gram, ParseTable PT)
{
	if(shead->top->stackSymbol.isNonTerminal)
	{
		//expandStack(): get prodcution grammar for this nonterm
		int grammarLine = PT.array[shead->top->stackSymbol.nontermValue][tk];
		while(gram->ruleNum != grammarLine){ //!MODIFY! grammar line and ruleNum have some format?
			gram= gram->nextRule;
		}
		// stackHead* shcurr = shead;
		terminal temp = gram->termSet->term_value;
		pop(shead); //remove the T from stack for rule T==> A B C
		SL* inserttop = malloc(sizeof(SL));
		SL* insertCurr = inserttop;
		inserttop->stackSymbol = temp;
		inserttop->below = NULL;

		while(gram->termSet->nextTerm != NULL)
		{
			temp = gram->termSet->nextTerm->term_value;
			SL* layer = malloc(sizeof(SL));
			layer->stackSymbol = temp;
			layer->below = NULL;
			insertCurr->below = layer;
			insertCurr = insertCurr->below;
			gram->termSet = gram->termSet->nextTerm;
		}		
		
		insertCurr->below = shead->top;
		shead = inserttop;

	}
	else //top of Stack is a terminal token
	{
		//match to tree
		//remove token from top of stack , and put in tree as leaf.
		addLeadToTree(pop(shead)); //!MODIFY! nned to define this func
	}
}

//////////////////STACK ADT ENDS/////////////////////////////


		void parseGrammar(struct _nonterm **nonterm, ParseTable PT)
		{
				// int grammarLine = 0;

				//int PT[/*no of non terms*/][/*no of terminals*/];
				//get these values from enum data type size

					// received nonterm
					
					int nontermNum = 41; //get these vals !MODIFY!
					int termNum = 41;
					int PT[nontermNum][termNum];
					int m,n;
					for(m=0; m<nontermNum ;m++){
						for(n=0; n<termNum ;n++){
							PT[m][n] = 0; //0 values indicate 'error' if not populated
						}
					}

					// ////////////////////////////////////////////////////////////////////////////
					// 
					// METHOD : A
					// 
					// not using grammar directly, just going through all non terminals and 
					// adding their first set vars to rules in parsing table. 
					if(nonterm !=NULL)
					{	
						int i,j;
						for(i=0;i<nontermNum; i++)
						{
							for(j=0; j < termNum; j++)
							{
								//check if term posn null nonterm[i].firstSet[j]
								if(nonterm[i].firstSet[j] != NULL)
									PT[nonterm[i].nonterm_value][nonterm[i].firstSet[j].token_name] = nonterm[i].firstSet[j].ruleNum;
								if(nonterm[i].nullable)
									PT[nonterm[i].nonterm_value][nonterm[i].followSet[j].token_name] = nonterm[i].followSet[j].ruleNum;
							}	
						}
					}
		}
		
			////////////////////////////////////////////////////////////////////////////////
			
		
			///////////////////////////////////////////////////////////////////////////////
			///
			/// METHOD : BK
			/// 
			/// use grammar each line and rules for making table
		// while(fGRAMMAR != EOF)
		// {
		// 	char *linestream= (char*)malloc(LINESIZE*sizeof(char));
		// 	if(fgets(linestream,LINESIZE,fGRAMMAR) == NULL) //gets a line from file to linestream
		// 	{ fprintf(stderr,"error reading line: '%s' : %s\n", linestream, strerror(errno)); continue;}
		// 	grammarLine++;
		// 	char *LHS = (char*)malloc(sizeof(char)*VSIZE);
		// 	char *RHSterm = (char*)malloc(sizeof(char)*VSIZE);
		// 	int LHSname = -1; //default
		// 	int RHSname = -1;
			
			
		// 	sscanf(linestream, "%s ; %s", LHS, RHSterm ); //string scanf in format
		// 	LHSname = getNonTerminal(LHS);
		// 	RHSname = getNonTerminal(RHS); //what if? RHS is a terminal instead? !MODIFY!
		// 	if(RHSname == invalidNonTerm)
		// 		{	//it may be a terminal then
		// 			RHSname = getToken(RHS);
		// 			//if so then add this rule to parsing table for the approp place and continue
		// 			PT[LHSname][RHSname] = grammarLine;
		// 			continue; //continue to next iteration of while loop
		// 		}

		// 	//find FIRST(RHS), for each e in the FIRSTset, add this grammarrule to PT[LHS(=NT),e]
		// 	//get non terminal for getNonTerminal(LHS), and the firstSet attrib of that.
			
		// 	// !MODIFY! need the first and follow grammar also written in this format, the NT should be enclosed in [] like for grammar
			 
		// 	int emptyTransfound = 0;
  // 			int dollarfound = 0;
  			
		//   	int j=0;
		// 	for(; j<nontermNum ;j++)
		//    {
		//       if(nonterm[RHSname].firstSet[j] != NULL) 
		//       {	  
		//       	 //op
		//       	 //		!MODIFY! this wont work. need to check all tokens till end to see if empty transition
		// 	      PT[LHSname][nonterm[RHSname].firstSet[j].token_name] = grammarLine; //production no
		// 	      if(nonterm[RHSname].firstSet[j].token_name == NIL)
		// 	      	{ emptyTransfound = 1;}
		// 	  }
		// 	}
  			

  // 			if(emptyTransfound)
  // 			{	
		// 	  	int j=0;
		// 		for( ; j<nontermNum;j++)
		// 	   	{
		// 	      if(nonterm[LHSname].followSet[j] != NULL) 
		// 	      {
		// 	      	//op
		// 		      PT[LHSname][nonterm[LHSname].followSet[j].token_name]] = grammarLine; //production no
		// 		      if(nonterm[LHSname].followSet[j].token_name == DOL)
		// 		      	{ dollarfound = 1;}
		// 	   	  }
			      
		// 		}
	  			
  // 			}

  // 			if(emptyTransfound && dollarfound)
  // 			{
  // 				PT[LHSname][DOL] = grammarLine; //production no
  // 			}
		// }

	

		//parsing table PT built. but not returned.



// int getWords(char a[], char *array[])
// {  
//   int i;
//   //char *array[50];
//   array[0]=strtok(a," ");
//   if(array[0]==NULL)
//    {
//      return 0; //fail
//    }

//   for(i=1;i<50;i++)
//    {
//     array[i]=strtok(NULL," ");
//     if(array[i]==NULL)
//       break;
//    }
//   return 1; //success
// }


parseTree  parseInputSourceCode(char *testcaseFile, ParseTable T)
{
	/**
	 * : This function takes as input the source code file and parses using the rules as per the 
	 * predictive parse table T. The function gets the tokens using lexical analysis interface 
	 * and establishes the syntactic structure of the input source code using rules in T. The 
	 * function must report errors as per the list of error specifications (will be provided to you soon) 
	 * if the source code is syntactically incorrect. If the source code is correct then the token and all
	 *  its relevant information is added to the parse tree. The start symbol of the grammar is the root 
	 *  of the parse tree and the tree grows as the syntax analysis moves in top down way. The function
	 *   must display a message " Compiled Successfully: Input source code is syntactically correct!!"
	 *   for successful parsing.
	 **/



}

void main()
{
	// FILE* fGRAMMAR = fopen("DUMMY_GRAMMAR.txt","r");//must check if non-null value is returned
	// FILE* fFIRST = fopen("DUMMY_FIRST.txt","r");//must check if non-null value is returned
	// FILE* fFOLLOW = fopen("DUMMY_FOLLOW.txt","r");//must check if non-null value is returned
	// if(fGRAMMAR == NULL || fFOLLOW == NULL || fFIRST == NULL)
	// {
	// 	printf("error loading files\n");
	// }
	// else
	// {
	// 	parseGrammar(fGRAMMAR, fFOLLOW,fFIRST); //should return ParseTable: int[][]
	// }



	ParseTable PT;// = malloc(sizeof(ParseTable));
	PT.rowSize = 41;
	PT.colSize = 41;
	PT.array = calloc(PT.rowSize, sizeof(int *)); 
	int i=0;
	for(;i<PT.colSize;i++)
		PT.array[i] = calloc(PT.colSize, sizeof(int)); //all cells initialised with zero

	//assuming i have grammar as nonterms list from the caller
	parseGrammar(nonterm, PT);

	
	
}