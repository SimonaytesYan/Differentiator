#ifndef __SYM_TREE_SETUP__
#define __SYM_TREE_SETUP__

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
    OP_LOG          = 6,
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

#include "BinaryTree/BinaryTree.h"

#endif