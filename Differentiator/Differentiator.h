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
    PLUS_OP = 0,
    SUB_OP  = 1,
    MUL_OP  = 2,
    DIV_OP  = 3,
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

const int  MAX_STR_LEN         = 20;

const char DEFAULT_TREE_NAME[] = "Tree";

int  SaveTreeInFile(Tree* tree, const char file_name[]);

void GetNodeFromFile(Node* node, void* fp_void);

int  GetTreeFromFile(Tree* tree, const char file_name[]);

void OutputGraphicDump(Tree* tree);

#endif //__DIFFERENTIATOR_SYM__