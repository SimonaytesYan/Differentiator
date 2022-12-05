#include "TaylorDecomposition.h"
#include "../../Differentiator.h"
#include "../Simplifying/Simplifying.h"

Node* GetOneTaylorDecMember(Node* derivative, long long fact, int number_member,
                                   const char* variable, double var_value)
{
    Node* answer  = NodeCtorOp(OP_MUL);

    L(answer)     = NodeCtorOp(OP_DIV);
    LL(answer)    = GetFunctionValueAtPoint(derivative, variable, var_value);
    SimplifyNode(LL(answer));
    LR(answer)    = NodeCtorNum((double)fact);

    R(answer)     = NodeCtorOp(OP_POW);
    RL(answer)    = NodeCtorOp(OP_SUB);
    L(RL(answer)) = NodeCtorVar(variable);
    R(RL(answer)) = NodeCtorNum(var_value);
        
    RR(answer)    = NodeCtorNum(number_member);
    SimplifyNode(R(answer));
    
    return answer;
}

Node* TaylorDecomp(Node* function, const char* variable, double var_value, int dec_order)
{
    if (dec_order == 0)
        return GetFunctionValueAtPoint(function, variable, var_value); 
    Node* root = NodeCtorOp(OP_PLUS);

    L(root) = GetFunctionValueAtPoint(function, variable, var_value);

    Node**    cur_node   = &(R(root));
    Node*     derivative = CpyNode(function);
    long long fact       = 1;
    for(int i = 1; i < dec_order; i++)
    {
        fact       *= i;
        *cur_node   = NodeCtorOp(OP_PLUS);

        Node* old_der = derivative;
        derivative  = Diff(derivative);
        DeleteNode(old_der);

        SimplifyNode(derivative);

        L(*cur_node) = GetOneTaylorDecMember(derivative, fact, i, variable, var_value);

        cur_node = &R(*cur_node);
    }

    #ifdef DEBUG
        Tree tree = {};
        tree.root = derivative;
        GraphicDump(&tree);
    #endif
    Node* old_der = derivative;
    derivative  = Diff(derivative);
    DeleteNode(old_der);

    #ifdef DEBUG
        tree.root = derivative;
        GraphicDump(&tree);
    #endif

    fact       *= (long long)dec_order;
    SimplifyNode(derivative);
    
    #ifdef DEBUG
        tree.root = derivative;
        GraphicDump(&tree);
    #endif

    *cur_node    = GetOneTaylorDecMember(derivative, fact, dec_order, variable, var_value);
    DeleteNode(derivative);

    return root;
}

Node* GetFunctionValueAtPoint(Node* function, const char* variable, double var_value)
{
    if (function == nullptr)
        return nullptr;
    
    Node* new_node = nullptr;

    switch (TYPE(function))
    {
        case TYPE_OP:
        {
            Node* left_node  = GetFunctionValueAtPoint(L(function), variable, var_value);
            Node* right_node = GetFunctionValueAtPoint(R(function), variable, var_value);

            new_node         = NodeCtorOp(VAL_OP(function));
            new_node->left   = left_node;
            new_node->right  = right_node;
            break;
        }
        case TYPE_NUM:
        {
            new_node = NodeCtorNum(VAL_N(function));
            break;
        }
        case TYPE_VAR:
        {
            if (!strcmp(variable, VAL_VAR(function)))
                new_node = NodeCtorNum(var_value);
            else
                new_node = NodeCtorVar(VAL_VAR(function));
            break;
        }

        case UNDEF_NODE_TYPE:
        default:
            break;
    }

    SimplifyNode(new_node);

    return new_node;
}