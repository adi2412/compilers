/* Compilers Project Group 2
// Aditya Raisinghani 2011A7PS042P
// Utkarsh Verma 2011A7PS137P
// BITS Pilani, Pilani Campus
// Second semester 2014
*/

// Contains functions for code generation

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tokens.h"
#include "lexer.h"
#include "parserDef.h"
#include "first_follow_gen.h"
#include "parser.h"
#include "symbol_table.h"
#include "type_extractor.h"
#include "codegen.h" //include in all others.

#define REGNUM 8		//no of registers = 8
int asmlineno = 0;
ASMList acode;

int regCount = 0;
int regBcount = 0;

void codegenInit(ASMList asmlist)
{	
	
	char *modeltext = ".MODEL SMALL"
	char *stacktext = ".STACK 100h"
	asmlist[asmlineno] 	 = malloc(sizeof(char)*LINESIZE);
	strcpy(asmlist[asmlineno],modeltext); 		//line0
	asmlist[++asmlineno] = malloc(sizeof(char)*LINESIZE);
	strcpy(asmlist[asmlineno],stacktext);		//line1
	
}

void codegenData(ASMList asmlist, IRTable headIR)
{
	char *datainit = ".DATA"			
	asmlist[++asmlineno] = malloc(sizeof(char)*LINESIZE);
	strcpy(asmlist[asmlineno],datainit);
	IRTable findIR = headIR;
	while(findIR != NULL)
	{
		if(findIR->op == MOV) 		//from enumerated ops list
		{

			char *datatext = malloc(sizeof(char)*LINESIZE);
			//// cat findIR->result.temp_name;
			//check if int or real or str
			if(findIR->arg1.typename  == INTEGER )
			{
				///char *datatext_type = " DW "; 		//fixed for now.
				///char *datatext_val = findIR->arg1.value; //try strcpy if this fails
				sprintf(datatext, "%s DW %d", findIR->result.temp_name, findIR->arg1.value);
			}
			/* FLOAT handling in ASM?
			else if(findIR->arg1.typename  == FLOAT ) 
			{
				char *datatext_type = " DW "; 		//fixed for now.
				char *datatext_val = findIR->arg1.floatValue;
			}
			*/
			else if(findIR->arg1.typename == STRINGENUM)	//replace with enum name
			{
				char *datatext_type = " DW ";
				//char *quote = "' ";
				//char *datatext_val;
				///strcpy(datatext_val,quote);
				///strcat(datatext_val, findIR->arg1.strValue);
				///strcat(datatext_val, quote);
				sprintf(datatext, "%s DW '%s' ", findIR->result.temp_name, findIR->arg1.strValue);
			}
			///strcat(datatext, datatext_type);
			///strcat(datatext, datatext_val);


			asmlist([++asmlineno]) = malloc(sizeof(char)*LINESIZE);
			strcpy(asmlist[asmlineno], datatext); 		//line added

			findIR = findIR->nextEntry;				//move to next entry
		}
	}
	
}

///Uses a round robin approach for register allocation:
///will return an occupied reg after all are occupied, in RR fashion.
char* findFreeReg()
{
	char *rname;
	switch(regCount)
	{
		case 0:
			rname = "ax";
			regCount++;
			break;
		case 1:
			rname = "bx";
			regCount++;
			break;
		case 2:
			rname = "cx";
			regCount++;
			break;
		case 3:
			rname = "dx";
			regCount = 0;
			break;
		default:
			rname = NULL;
	}	
	return rname;
}

char* findFreeRegBIG()
{
	char *rBname;
	switch(regBcount)
	{
		case 0:
			rBname = "ds, "; //note the , reqd in dest.
			regBcount++;
			break;
		case 1:
			rBname = "es, ";
			regBcount = 0;
			break;
		//add more CS,SS if reqd.
		default:
			rBname = NULL;
	}
	return rBname;
}

char* insertBwStr(char* t1, char* t2, char* t3)
{
	char *res;
	strcpy(res,t1);
	strcat(res,t2);
	strcat(res,t3);
	return res;
}

void insertASMline(ASMList asmlist, char *str)
{
	asmlist([++asmlineno]) = malloc(sizeof(char)*LINESIZE);
	strcpy(asmlist[asmlineno], str);  
}


void codegenOps(ASMList asmlist, IRTable headIR)
{
	char *opsinit = ".CODE"			
	asmlist[++asmlineno] = malloc(sizeof(char)*LINESIZE);
	strcat(asmlist[asmlineno],opsinit);
	IRTable findIR = headIR;
	while(findIR != NULL)
	{
		//for MOV op,need to load the val to a reg
		if(findIR->op == MOV) 		//from enumerated ops list
		{
			char *reg = findFreeReg();
			char *mov_offset = insertBwStr("mov ",reg, ", @data");  //if this doesnt work, then put t1 and t3 as sep strings and pass.
			//asmlist([++asmlineno]) = malloc(sizeof(char)*LINESIZE);
			//strcpy(asmlist[asmlineno], mov_offset);   
			insertASMline(asmlist, mov_offset);

			char *regB = findFreeRegBIG();
			char *mov_ds = insertBwStr("mov ",regB, reg);
			insertASMline(asmlist, mov_ds);

			char *mov_dsoffset = insertBwStr("mov ",regB,"OFFSET ");
			strcat(mov_dsoffset, findIR->result.temp_name);
			insertASMline(asmlist, mov_dsoffset);
					
			findIR = findIR->nextEntry;
		}
		else if(findIR->op == ADD ) // || MUL ,SUB,DIV others
		{
			//! MODIFY. mismatch possbile btw loaded reg and this. think .
			//! this may fail if the regs reqd are overwritten.
			//! bascially if the math op is just after the mov ops, then fine
			
			char *reg = findFreeReg();
			
			char *math_text = insertBwStr("mov ",reg, ", " );
			strcat(math_text, findIR->arg1.temp_name);
			insertASMline(asmlist, math_text);

			char *math_text2 = insertBwStr("add ",reg, ", ");
			strcat(math_text2 , findIR->arg2.temp_name);
			insertASMline(asmlist, math_text2);

			char *math_text3 = insertBwStr("mov ", findIR->result.temp_name, ", ");
			strcat(math_text3, reg);
			insertASMline(asmlist , math_text3);

			insertASMline(asmlist, math_text);
			findIR = findIR->nextEntry;

		}
		/* mak add work for all other 3 ops. use automation.
		else if(findIR->op == SUB ) // || MUL ,SUB,DIV others
		{
			//! MODIFY. mismatch possbile btw loaded reg and this. think .
			char *math_text= "sub ";
			strcat(math_text, findIR->result.temp_name);
			strcat(math_text, findIR->arg1.temp_name);
			strcat(math_text, findIR->arg2.temp_name);

			insertASMline(asmlist, math_text);

			findIR = findIR->nextEntry;

		}
		else if(findIR->op == MUL ) // || MUL ,SUB,DIV others
		{
			//! MODIFY. mismatch possbile btw loaded reg and this. think .
			char *math_text= "mul ";
			strcat(math_text, findIR->result.temp_name);
			strcat(math_text, findIR->arg1.temp_name);
			strcat(math_text, findIR->arg2.temp_name);

			insertASMline(asmlist, math_text);
			findIR = findIR->nextEntry;

		}
		else if(findIR->op == DIV ) // || MUL ,SUB,DIV others
		{
			//! MODIFY. mismatch possbile btw loaded reg and this. think .
			char *math_text= "div ";
			strcat(math_text, findIR->result.temp_name);
			strcat(math_text, findIR->arg1.temp_name);
			strcat(math_text, findIR->arg2.temp_name);

			insertASMline(asmlist, math_text) ;

			findIR = findIR->nextEntry;

		}	
		*/
}

void writeCodetoFile()
{
	FILE* outfile = fopen("code.asm","rw");
	int c = 0;
	if( outfile != NULL)
	{
		while(c<= asmlineno)
		{
			fprintf(outfile, "%s\n", asmlist[c]);
		}

	}
}


void mainCodeGen()
{
	acode = malloc(siezeof(codeLine)*ASMLINES);

	if(acode != NULL)
	{
		codegenInit(acode);
		codegenData(acode, headIR);
		codegenOps( acode, headIR);
		writeCodetoFile();	
	}
	
}