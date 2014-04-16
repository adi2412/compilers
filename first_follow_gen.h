#ifndef FFG
#define FFG


nonterm ffg(FILE *fp);
rule readAndDefineGrammarRule(FILE *fp);
first computeFirst(rule gramRule, nonterm head);
follow computeFollow(rule gramRule, nonterm head);
nonterm initialiseNonTerminals();
char* getNonTermValue(nonTerminal nonterm);

#endif