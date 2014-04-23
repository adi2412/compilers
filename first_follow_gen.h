/* Compilers Project Group 2
// Aditya Raisinghani 2011A7PS042P
// Utkarsh Verma 2011A7PS137P
// BITS Pilani, Pilani Campus
// Second semester 2014
*/

#ifndef FFG
#define FFG


nonterm ffg(FILE *fp);
rule readAndDefineGrammarRule(FILE *fp);
first computeFirst(rule gramRule, nonterm head);
follow computeFollow(rule gramRule, nonterm head);
nonterm initialiseNonTerminals();
char* getNonTermValue(nonTerminal nonterm);
int getNonTerminal(char* term);
token getToken(char* term);
#endif