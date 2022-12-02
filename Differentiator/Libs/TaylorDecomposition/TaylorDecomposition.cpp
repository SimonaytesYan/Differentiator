#include "TaylorDecomposition.h"
#include "../../Differentiator.h"
#include "../Simplifying/Simplifying.h"

Node* GetOneTaylorDecMember(Node* derivative, long long fact, int number_member,
                                   const char* variable, double var_value)
{
    Node* answer  = NodeCtorOp(OP_MUL);

    L(answer)     = NodeCtorOp(OP_DIV);
    LL(answer)    = GetFunctionValueAtPoint(derivative, variable, var_value);
    LR(answer)    = NodeCtorNum((double)fact);

    R(answer)     = NodeCtorOp(OP_POW);
    RL(answer)    = NodeCtorOp(OP_SUB);
    char* new_var = (char*)calloc(strlen(variable) + 1, 1);
    strcpy(new_var, variable);
    
    L(RL(answer)) = NodeCtorVar(new_var);
    R(RL(answer)) = NodeCtorNum(var_value);
        
    RR(answer)    = NodeCtorNum(number_member);
    
    return answer;
}

Node* TaylorDecomp(Node* function, const char* variable, double var_value, int dec_order)
{
    if (dec_order == 0)
        return GetFunctionValueAtPoint(function, variable, var_value); 
    Node* root = NodeCtorOp(OP_PLUS);

    L(root) = GetFunctionValueAtPoint(function, variable, var_value);

    Node**    cur_node   = &(R(root));
    Node*     derivative = function;
    long long fact       = 1;
    for(int i = 1; i < dec_order; i++)
    {
        fact       *= i;
        *cur_node   = NodeCtorOp(OP_PLUS);
        derivative  = Diff(derivative);
        SimplifyNode(derivative);

        L(*cur_node) = GetOneTaylorDecMember(derivative, fact, i, variable, var_value);
        SimplifyNode(L(*cur_node));

        cur_node = &R(*cur_node);
    }

    derivative  =  Diff(derivative);
    fact       *= (long long)dec_order;
    SimplifyNode(derivative);

    *cur_node    = GetOneTaylorDecMember(derivative, fact, dec_order, variable, var_value);

    SimplifyNode(*cur_node);
    
    SimplifyNode(root);

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
            {
                char *new_var = nullptr;
                strcpy(new_var, VAL_VAR(new_node));
                new_node = NodeCtorVar(new_var);
            }
            break;
        }

        case UNDEF_NODE_TYPE:
        default:
            break;
    }

    SimplifyNode(new_node);

    return new_node;
}