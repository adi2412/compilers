/* Compilers Project Group 2
// Aditya Raisinghani 2011A7PS042P
// Utkarsh Verma 2011A7PS137P
// BITS Pilani, Pilani Campus
// Second semester 2014
*/

#ifndef CODEG
#define CODEG

//////////////HEADER//////////////////

#define ASMLINES 200
#define LINESIZE 35

typedef struct _codeLine *codeLine;
typedef codeLine* ASMList;

struct _codeLine
{
	char* asmtext;
	codeLine next;
};

/////////////////////////////////////

#endif


//////////REGISTERS///////////////
/*
_________________
|	AH	|	AL 	|
|	DH	|	DL	|
|	CH 	| 	CL 	|
|	BH	|	BL	|
|		BP		|
|		SI		|
|		DI 		|
|_______SP______|

*/