#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "Differentiator.h"
#include "Libs/DSL.h"
#include "Libs/ResursiveDescent/RecursiveDescent.h"
#include "Libs/LatexOutput/LatexOutput.h"
#include "Libs/Simplifying/Simplifying.h"

//#define DEBUG

//--------------------FUNCTION PROTOTIPES--------------------

static bool  IsNodeConst(Node* node);

static Node* DiffDiv(Node* node_arg);

static Node* DiffMult(Node* node_arg);

static Node* DiffSin(Node* node_arg);

static Node* DiffCos(Node* node_arg);

static Node* DiffSum(Node* node_arg);

static Node* DiffSub(Node* node_arg);

//--------------------FUNCTION IMPLEMENTATION--------------------

static bool IsNodeConst(Node* node)
{
    if (node == nullptr)
        return true;
    
    if (TYPE(node) == TYPE_VAR)
        return false;
    
    if (!IsNodeConst(L(node)))
        return false;
    if (!IsNodeConst(R(node)))
        return false;

    return true;
}

static Node* DiffDiv(Node* node)
{
    Node* new_node = NodeCtorOp(OP_DIV);

    L(new_node) = NodeCtorOp(OP_SUB);
    R(new_node) = NodeCtorOp(OP_MUL);

    LL(new_node) = NodeCtorOp(OP_MUL); 
    LR(new_node) = NodeCtorOp(OP_MUL);

    L(LL(new_node)) = Diff(L(node));
    R(LL(new_node)) = CpyNode(R(node));
            
    L(LR(new_node)) = Diff(R(node));
    R(LR(new_node)) = CpyNode(L(node));

    RR(new_node) = CpyNode(R(node));
    RL(new_node) = CpyNode(R(node));

    #ifdef DEBUG
        printf("Div\n");
    #endif

    ReturnAndTex;
}

static Node* DiffMult(Node* node)
{
    Node* new_node   = NodeCtorOp(OP_PLUS);

    L(new_node) = NodeCtorOp(OP_MUL);
    R(new_node) = NodeCtorOp(OP_MUL);

    LL(new_node) = Diff(L(node));
    LR(new_node) = CpyNode(R(node));

    RL(new_node) = CpyNode(L(node));
    RR(new_node) = Diff(R(node));
    
    #ifdef DEBUG
        printf("Mult\n");
    #endif

    ReturnAndTex;
}

static Node* DiffSin(Node* node)
{
    Node* new_node = NodeCtorOp(OP_MUL);

    L(new_node) = Diff(R(node));
    R(new_node) = NodeCtorOp(OP_COS);

    RL(new_node) = nullptr;
    RR(new_node) = CpyNode(R(node));
    
    #ifdef DEBUG
        printf("Sin\n");
    #endif

    ReturnAndTex;
}

static Node* DiffCos(Node* node)
{
    Node* new_node = NodeCtorOp(OP_MUL);

    L(new_node)  = Diff(R(node));
    R(new_node)  = NodeCtorOp(OP_SUB);

    RR(new_node) = NodeCtorOp(OP_SIN);
    RL(new_node) = NodeCtorNum(0);

    L(RR(new_node)) = nullptr;
    R(RR(new_node)) = CpyNode(R(node));
    
    #ifdef DEBUG
        printf("Cos\n");
    #endif

    ReturnAndTex;
}

static Node* DiffSum(Node* node)
{
    Node* new_node  = NodeCtorOp(OP_PLUS);

    new_node->left  = Diff(L(node));
    new_node->right = Diff(R(node));
    
    #ifdef DEBUG
        printf("Sum\n");
    #endif

    ReturnAndTex;
}

static Node* DiffSub(Node* node)
{
    Node* new_node  = NodeCtorOp(OP_SUB);

    L(new_node) = Diff(L(node));
    R(new_node) = Diff(R(node));
                
    #ifdef DEBUG
        printf("Sub\n");
    #endif
    ReturnAndTex;
}

static Node* DiffLog(Node* node)
{
    Node* new_node = NodeCtorOp(OP_MUL);

    L(new_node)  = NodeCtorOp(OP_DIV);
    LL(new_node) = NodeCtorNum(1);

    LR(new_node) = CpyNode(R(node));
    R(new_node)  = Diff(R(node));
    
    #ifdef DEBUG
        printf("Log\n");
    #endif

    ReturnAndTex;
}

static Node* DiffPow(Node* node)
{
    Node* new_node = nullptr;

    bool const_basis     = IsNodeConst(L(node));
    bool const_indicator = IsNodeConst(R(node));

    if (const_basis)
    {
        if (const_indicator)
        {
            new_node = NodeCtorNum(0);
        }
        else
        {
            new_node = NodeCtorOp(OP_MUL);
            
            R(new_node) = Diff(R(node));

            L(new_node)     = NodeCtorOp(OP_MUL);
            LR(new_node)    = CpyNode(node);
            LL(new_node)    = NodeCtorOp(OP_LOG);
            R(LL(new_node)) = CpyNode(R(node));
        }
    }
    else if (const_indicator)
    {
        new_node = NodeCtorOp(OP_MUL);
        R(new_node) = Diff(L(node));

        L(new_node)     = NodeCtorOp(OP_MUL);
        LL(new_node)    = NodeCtorOp(OP_SUB);
        L(LL(new_node)) = CpyNode(R(node));
        R(LL(new_node)) = NodeCtorNum(1);
            
        LR(new_node)     = NodeCtorOp(OP_POW);
        L(LR(new_node))  = CpyNode(L(node));
        R(LR(new_node))  = NodeCtorOp(OP_SUB);
        RL(LR(new_node)) = CpyNode(R(node));
        RR(LR(new_node)) = NodeCtorNum(1);
    }
    else
    {
        new_node = NodeCtorOp(OP_MUL);

        L(new_node)  = CpyNode(node);
        R(new_node)  = NodeCtorOp(OP_PLUS);

        RL(new_node) = NodeCtorOp(OP_MUL);
        RR(new_node) = NodeCtorOp(OP_MUL);

        L(RL(new_node)) = NodeCtorOp(OP_DIV);
        R(RL(new_node)) = Diff(L(node));

        LL(RL(new_node)) = CpyNode(R(node));
        LR(RL(new_node)) = CpyNode(L(node));

        L(RR(new_node))  = Diff(R(node));
        R(RR(new_node))  = NodeCtorOp(OP_LOG);
        RR(RR(new_node)) = CpyNode(L(node));
    }

    #ifdef DEBUG
        printf("Pow\n");
    #endif
    ReturnAndTex;
}

Node* Diff(Node* node)
{
    assert(node);

    if (IS_NUM(node))
    {
        Node* new_node = NodeCtorNum(0);
        ReturnAndTex;
    }

    if (IS_VAR(node))
    {
        Node* new_node = NodeCtorNum(1);
        ReturnAndTex;
    }

    if (IS_OP(node))
    {
        switch (VAL_OP(node))
        {
            case OP_PLUS:
                return DiffSum(node);
            case OP_SUB:
                return DiffSub(node);
            case OP_MUL:
                return DiffMult(node);
            case OP_DIV:
                return DiffDiv(node);
            case OP_SIN:
                return DiffSin(node);
            case OP_COS:
            {
                return DiffCos(node);
            }
            case OP_LOG:
                return DiffLog(node);
            case OP_POW:
                return DiffPow(node);

            case UNDEF_OPER_TYPE:
            {
                LogPrintf("Undefined operation type while diff\n");
                return nullptr;
            }
            
            default:
            {
                LogPrintf("Unknown operation type while diff\n"
                        "Operation type = %d", VAL_OP(node));            
                return nullptr;
            }
        }
    }

    LogPrintf("Unknown node type\n"
              "Node type = %d\n", TYPE(node));
    return nullptr;
}
