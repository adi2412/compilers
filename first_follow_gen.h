#ifndef FFG
#define FFG


int ffg(FILE *fp);
rule readAndDefineGrammarRule(FILE *fp);
first computeFirst(rule gramRule, nonterm head);
follow computeFollow(rule gramRule, nonterm head);
nonterm initialiseNonTerminals();

#endif