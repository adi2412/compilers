#ifndef TYPEEXT
#define TYPEEXT

token checkTypeInSymbolTable(char* name,int lineNum,int charNum);
token convertToType(token type);
void runTypeChecker();
void runTypeCheckerInIfScope();
int typeChecker(astTree astRoot, STList headList);

#endif