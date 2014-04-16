
typedef struct _parseTable{	
	int rowSize;
	int colSize;
	int **array; 
} ParseTable;

typedef ParseTable *table;

//////////////////////TREE FOLLOWS//////////////////
typedef struct _treenode *TN; 
// typedef TN* children;
typedef enum isNonTerm flag;

struct _treenode
{	
	flag isLeaf;
	tokenInfo token;		//the data contained is a token
	NT nonTerminal;
	TN sisterNode;
	TN parent;
	TN childNode; //leftmost node
};

typedef struct _treeroot{
	TN root;
	TN currNode; //currentNode
} *treeroot;

typedef treeroot parseTree;

typedef struct _stackLayer *SL;
struct _stackLayer{
	terminal stackSymbol;
	SL below;
};

typedef struct _stackHead{
	int size;//current size of stack
	SL top;
	SL bottom;
} *stackHead;