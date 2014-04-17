#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parserDef.h"
#include "lexer.h"
#include "first_follow_gen.h"
#include "parser_adi.h"
#define VSIZE 30 
#define LINESIZE 512
#define EMPTY -1

grammar gram;
treeroot tr;

table T;
nonterm nonTerm;

void generateError()
{

}

treeroot createTree()
{
	treeroot _tr = malloc(sizeof(treeroot));
	_tr->root = NULL;
	_tr->currNode = NULL;
	return _tr;
}

void addSisterNodes(terminals terms, tokenInfo tk)
{
	TN node = malloc(sizeof(struct _treenode));
	tr->currNode->childNode = node;
	while(terms != NULL)
	{
		node->isLeaf = !(terms->term_value.flag);
		if(node->isLeaf)
		{
			node->token->token_name = terms->term_value.tokValue;
		}
		node->token = tk;
		node->parent = tr->currNode;
		node->childNode = NULL;
		node->sisterNode = malloc(sizeof(struct _treenode));
		node = node->sisterNode;
		terms = terms->nextTerm;
	}
}

void addLeafToTree(terminal term,tokenInfo tk)
{
	TN leafNode = malloc(sizeof(struct _treenode));
	tr->currNode->childNode = leafNode;
	leafNode->parent = tr->currNode;
	leafNode->token = tk;
	leafNode->isLeaf = TRUE;
	leafNode->sisterNode = NULL;
}

stackHead createStack()
{
	stackHead sh = malloc(sizeof(struct _stackHead));
	sh->top = NULL;
	sh->bottom = NULL;
	sh->size = 0;
	return sh;
}

int isEmpty(stackHead shead)
{
	if(shead->bottom == NULL)
		return 1; // yes, is empty
	else
		return 0; //non empty
}

terminal topStack(stackHead shead)
{
	return shead->top->stackSymbol;
}

stackHead push(terminal t, stackHead shead )
{
	SL layer = malloc(sizeof(struct _stackLayer));
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
	shead->size++;
	return shead;
}

stackHead pop(stackHead shead)
{
	SL del = shead->top;
	shead->top = shead->top->below;
	shead->size--;
	free(del);
	return shead;
}

stackHead matchStack(stackHead shead, tokenInfo tk,rule gram, int gramLine)
{
	if(shead->top->stackSymbol.flag)
	{
		//expandStack(): get prodcution grammar for this nonterm
		int grammarLine = gramLine;
		if(grammarLine == 0)
		{
			generateError();
		}
		while(gram->ruleNum != grammarLine){ //!MODIFY! grammar line and ruleNum have some format?
			gram = gram->nextRule;
		}
		// stackHead* shcurr = shead;
		terminals temp = gram->termSet;
		terminals headTemp = temp;
		pop(shead); //remove the T from stack for rule T==> A B C
		addSisterNodes(temp,tk);
		SL inserttop = malloc(sizeof(struct _stackLayer));
		SL insertCurr = inserttop;
		inserttop->stackSymbol = temp->term_value;
		inserttop->below = NULL;

		while(temp->nextTerm != NULL)
		{
			temp = gram->termSet->nextTerm;
			SL layer = malloc(sizeof(struct _stackLayer));
			layer->stackSymbol = temp->term_value;
			layer->below = NULL;
			insertCurr->below = layer;
			insertCurr = insertCurr->below;
			gram->termSet = gram->termSet->nextTerm;
		}		
		
		insertCurr->below = shead->top;
		shead->top = inserttop;

	}
	else //top of Stack is a terminal token
	{
		terminal term = topStack(shead);
		if(term.tokValue != tk->token_name)
		{
			generateError();
		}
		else
		{
			// It is correct token.
			shead = pop(shead);
			addLeafToTree(term,tk);
		}
	}
	return shead;
}

//////////////////STACK ADT ENDS/////////////////////////////


void parseGrammar()
{
// int grammarLine = 0;

//int PT[/*no of non terms*/][/*no of terminals*/];
//get these values from enum data type size

	// received nonterm
	
	int nontermNum = T->rowSize; //get these vals !MODIFY!
	int termNum = T->colSize;
	T->array = malloc(T->rowSize*T->colSize*sizeof(int));
	int m,n;
	for(m=0; m<nontermNum ;m++){
		for(n=0; n<termNum ;n++){
			T->array[m][n] = 0; //0 values indicate 'error' if not populated
		}
	}

	// ////////////////////////////////////////////////////////////////////////////
	// 
	// METHOD : A
	// 
	// not using grammar directly, just going through all non terminals and 
	// adding their first set vars to rules in parsing table. 
	if(nonTerm !=NULL)
	{	
		int i,j;
		for(i=0;i<nontermNum; i++)
		{
			for(j=0; j < termNum; j++)
			{
				//check if term posn null nonterm[i].firstSet[j]
				if(nonTerm[i]->firstSet[j] != NULL)
					T->array[nonTerm[i]->nonterm_value][nonTerm[i]->firstSet[j]->token_name] = nonTerm[i]->firstSet[j]->ruleNum;
				if(nonTerm[i]->nullable)
					T->array[nonTerm[i]->nonterm_value][nonTerm[i]->followSet[j]->token_name] = nonTerm[i]->followSet[j]->ruleNum;
			}	
		}
	}
}


void createParseTable()
{
	// Creates all the first sets for all the non-terminals
	FILE *fp = fopen("grammar_rules.txt","r");
	if(fp == NULL)
	{
		printf("The file could not be found.");
		exit(1);
	}
	// Read the first grammar rule
	rule gramRule = readAndDefineGrammarRule(fp);
	int ruleNum = 1;
	// Maintain a pointer at the first rule
	rule G = gramRule;
	while(gramRule != NULL)
	{
		// Keep computing first sets and reading grammar rules
		gramRule->ruleNum = ruleNum++;
		gramRule->nextRule = readAndDefineGrammarRule(fp);
		printf("Rule Number %d) %d-->",gramRule->ruleNum,gramRule->nonterm_value);
		terminals readterms = gramRule->termSet;
		while(readterms != NULL)
		{
			if(readterms->term_value.flag)
			{
				// Non terminal
				printf("%d\t",readterms->term_value.nontermValue);
			}
			else
			{
				printf("%d\t",readterms->term_value.tokValue);
			}
			readterms = readterms->nextTerm;
		}
		printf("\n");
		gramRule = gramRule->nextRule;
	}
	// Having read all the rules, start computing first sets.

	gramRule = G;
	
	nonTerm = initialiseNonTerminals();
	int i = gramRule->nonterm_value;
	while(i<NONTERMINALS)
	{
		// First and follow still need to be computed for this non terminal.
		if(nonTerm[i] != NULL)
		{
			if(!nonTerm[i]->nullable) nonTerm[i]->nullable = FALSE;
		}
		if(nonTerm[i] == NULL)
		{
			nonTerm[i] = malloc(sizeof(struct _nonterm));
		}
		nonTerm[i]->firstSet = computeFirst(gramRule,nonTerm);
		gramRule = gramRule->nextRule;
		if(gramRule == NULL) break;
		i = gramRule->nonterm_value;
	}

	gramRule = G;
	// Compute the follow sets.
	i = gramRule->nonterm_value;
	// Set dollar in follow of start symbol
	nonTerm[i]->followSet = malloc(TERMINALS*sizeof(struct _follow*));
	nonTerm[i]->followSet[DOL] = malloc(sizeof(struct _follow*));
	nonTerm[i]->followSet[DOL]->token_name = DOL;
	nonTerm[i]->followSet[DOL]->ruleNum = gramRule->ruleNum;
	while(i<NONTERMINALS)
	{
		// All non terminals have been malloc'd. Just start finding their follow
		nonTerm[i]->followSet = computeFollow(gramRule, nonTerm);
		gramRule = gramRule->nextRule;
		if(gramRule == NULL) break;
		i = gramRule->nonterm_value;
	}

	int j = 0;
	for(;j<NONTERMINALS-1;++j)
	{
		printf("For %d ",j);
		if(nonTerm[j]->nullable)
		{
			printf("(NULLABLE)----->\n");
		}
		else
		{
			printf("----->\n");
		}
		printf("First set:\t");
		int k =0;
		for(;k<TERMINALS;++k)
		{
			if(nonTerm[j]->firstSet[k] != NULL)
			{
				printf("%s\t",getTokenName(nonTerm[j]->firstSet[k]->token_name));
			}
		}
		printf("\n");
		printf("Follow set:\t");
		k = 0;
		for(;k<TERMINALS;++k)
		{
			if(nonTerm[j]->followSet[k] != NULL)
			{
				printf("%s(%d--%d)\t",getTokenName(nonTerm[j]->followSet[k]->token_name),nonTerm[j]->followSet[k]->ruleNum,k);
			}
		}
		printf("\n");
		printf("\n");
	}
	parseGrammar();
}

parseTree  parseInputSourceCode(FILE *testcaseFile, table T, nonterm nonTerm)
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
	 /* Temporary!!! */
	FILE* dest = fopen("parsetreeOutFile.txt","w");
	tokenInfo head = lexer(testcaseFile,dest);
	stackHead shead = createStack();
	terminal headTerminal;
	headTerminal.flag = FALSE;
	headTerminal.tokValue = DOL;	// Put dollar at bottom of stack
	shead = push(headTerminal,shead);
	tr = createTree();
	TN headNode = malloc(sizeof(struct _treenode));
	headNode->isLeaf = FALSE;
	headNode->nonTerminal = nonTerm[0];
	headNode->sisterNode = NULL;
	headNode->parent = NULL;
	headNode->childNode = NULL;
	tr->root = headNode;
	tr->currNode = headNode;
	tokenInfo readTokens = head;
	while(readTokens != NULL)
	{
		terminal topTerm = topStack(shead);
		if(topTerm.flag)
		{
			int ruleNum = T->array[topTerm.nontermValue][readTokens->token_name];
		}
		//matchStack(shead, readTokens, readTokens->token_name,  )
	}

	return NULL;
}

int main()
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



	// ParseTable PT;// = malloc(sizeof(ParseTable));
	// PT.rowSize = 41;
	// PT.colSize = 41;
	// PT.array = calloc(PT.rowSize, sizeof(int *)); 
	// int i=0;
	// for(;i<PT.colSize;i++)
	// 	PT.array[i] = calloc(PT.colSize, sizeof(int)); //all cells initialised with zero

	//assuming i have grammar as nonterms list from the caller
	//parseGrammar(nonTerm, PT);	

	return 0;
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
