//symbol table
/* Compilers Project Group 2
// Aditya Raisinghani 2011A7PS042P
// Utkarsh Verma 2011A7PS137P
// BITS Pilani, Pilani Campus
// Second semester 2014
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parserDef.h"
#include "tokens.h"
#include "lexer.h"
#include "first_follow_gen.h"
#include "parser.h"
#include "sem_parser.h"


#define TBSIZE 40


void makeSymbolTable()
{
	//Table tlist = malloc(sizeof(struct _tabelEntry*)* TBSIZE);
	Env e;
	e.table = malloc(sizeof(TableEntry));
	e.prev= NULL;

	Env e1 = makeScopedTable(&e.table);
	

}

Env makeScopedTable(Table* t)
{
	Env r;
	r.table = malloc(sizeof(TableEntry));
	r.prev = t;
	return r;
}

void insertEntrytoTable(char* k, token t_sym, Table t_ins)
{
	if(NULL == t_ins )
	{
		t_ins = malloc(sizeof(TableEntry));
		strcpy(t_ins->s,k);
		t_ins->sym = t_sym;
		t_ins->next = NULL;

	}
	else if(t_ins->s != NULL)
	{
		t_ins->next = malloc(sizeof(TableEntry));
		t_ins = t_ins->next;
		strcpy(t_ins->s,k);
		t_ins->sym = t_sym;
		t_ins->next = NULL;
	}
}

void deleteEntryfromTable(char*k , token t_sym, Table t_ins)
{

}