#ifndef __DIFFERENTIATOR_SYM__
#define __DIFFERENTIATOR_SYM__

#include "Libs/Logging/Logging.h"

enum NODE_TYPES
{
    UNDEF_NODE_TYPE = -1,
    TYPE_VAR        = 0,
    TYPE_OP         = 1,
    TYPE_NUM        = 2,
};

enum OPER_TYPES
{
    UNDEF_OPER_TYPE = -1,
    OP_PLUS         = 0,
    OP_SUB          = 1,
    OP_MUL          = 2,
    OP_DIV          = 3,
    OP_SIN          = 4,
    OP_COS          = 5,
    OP_LN           = 6,
    OP_POW          = 7,
};

struct Node_t 
{
    NODE_TYPES type = UNDEF_NODE_TYPE;
    struct
    {
        double      dbl = 0;
        OPER_TYPES  op  = UNDEF_OPER_TYPE; 
        char*       var = nullptr;
    } val;
};

void PrintElemInLog(Node_t elem);

#include "Libs/BinaryTree/BinaryTree.h"

const int   MAX_STR_LEN         = 20;

const char  DEFAULT_TREE_NAME[] = "Tree";

const char  DEFAULT_TEX_NAME[]  = "Zorich_3_volume.tex";

int   SaveTreeInFile(Tree* tree, const char file_name[]);

void  GetNodeFromFile(Node** new_node, FILE* fp);

int   GetTreeFromFile(Tree* tree, const char file_name[]);

void  OutputGraphicDump(Tree* tree);

int   OpenLatexFile(const char file_name[]);

int   TexNode(Node* root);

void  CloseLatexFile();

Node* Diff(Node* node);

Node* CpyNode(Node* node);

void  PrintfInLatex(const char *format, ...);

void  SimplifyTree(Tree* tree);

void  ConstsConvolution(Node* node);

void PrintfInLatexReal(const char* function, const char *format, ...);

int GetG(const char* str);

int GetE();

int GetT();

int GetP();

int GetN();

#define PrintfInLatex(format, ...) PrintfInLatexReal(__PRETTY_FUNCTION__, format,##__VA_ARGS__);

#endif //__DIFFERENTIATOR_SYM__