#ifndef SEMP
#define SEMP
semrule readAndDefineSemanticRule(FILE *fp);
char* readNonTerm(char* term, char* rule, int ptr);
sem readLeafTerm(char* rule, int ptr);
sems getRHSnodes(char* ruleString, int ptr, choice _ch);
semRuleArray returnSemanticRules();
#endif