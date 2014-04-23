/* Compilers Project Group 2
// Aditya Raisinghani 2011A7PS042P
// Utkarsh Verma 2011A7PS137P
// BITS Pilani, Pilani Campus
// Second semester 2014
*/

#ifndef TYPEEXT
#define TYPEEXT

token checkTypeInSymbolTable(char* name,int lineNum,int charNum);
token convertToType(token type);
void runTypeChecker();
void runTypeCheckerInIfScope();
int typeChecker(astTree astRoot, STList headList);

#endif