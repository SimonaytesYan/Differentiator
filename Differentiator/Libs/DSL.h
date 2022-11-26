#ifndef __SYM_DIFF_DSL__
#define __SYM_DIFF_DSL__

#define ReturnAndTex                            \
    PrintRandBundleInLatex();                   \
                                                \
    TexEqualityWithDesignations(node, new_node, \
             "\\begin{center}\n (", ")`\n = "); \
                                                \
    PrintfInLatex("\\end{center}\n");           \
    return new_node;

#define BinaryConstConv(oper)           \
{                                       \
    double a = VAL_N(L(node));          \
    double b = VAL_N(R(node));          \
                                        \
    DelLR(node);                        \
                                        \
    TYPE(node)  = TYPE_NUM;             \
    VAL_N(node) = a oper b;             \
}

#define UnaryConstConv(func)            \
{                                       \
    double a = VAL_N(R(node));          \
                                        \
    DelLR(node);                        \
                                        \
    TYPE(node)  = TYPE_NUM;             \
    VAL_N(node) = func(a);              \
}

#define CpyAndReplace(from_cpy, replace_it)                 \
{                                                           \
    PrintRandBundleInLatex();                               \
    TexEqualityWithDesignations(replace_it, from_cpy,       \
                                "\\begin{center}\n", "");   \
    PrintfInLatex("\\end{center}\n")                        \
                                                            \
    Node* replacement = CpyNode(from_cpy);                  \
                                                            \
    DelLR(replace_it);                                      \
    *replace_it = *replacement;                             \
    free(replacement);                                      \
}

#define DelLR(node)                             \
    DeleteNode(L(node));                        \
    DeleteNode(R(node));                        \
    L(node) = nullptr;                          \
    R(node) = nullptr;


#define L(node) node->left

#define R(node) node->right

#define LL(node) node->left->left

#define LR(node) node->left->right

#define RL(node) node->right->left

#define RR(node) node->right->right


#define IS_VAR(node) (node->val.type == TYPE_VAR)

#define IS_OP(node) (node->val.type == TYPE_OP)

#define IS_NUM(node) (node->val.type == TYPE_NUM)

#define IS_ZERO(node) (IS_NUM(node) && VAL_N(node) == 0)

#define IS_ONE(node) (IS_NUM(node) && VAL_N(node) == 1)


#define VAL_N(node) node->val.val.dbl

#define VAL_OP(node) node->val.val.op

#define VAL_VAR(node) node->val.val.var

#define TYPE(node) node->val.type


#define PUT_PLUS                    \
    case OP_PLUS:                   \
        fprintf(stream, " + ");     \
        break;

#define PUT_SUB                     \
    case OP_SUB:                    \
        fprintf(stream, " - ");     \
        break;
        
#define PUT_MUL                     \
    case OP_MUL:                    \
        fprintf(stream, " * ");     \
        break;
        
#define PUT_DIV                     \
    case OP_DIV:                    \
        fprintf(stream, " / ");     \
        break;

#define PUT_SIN                     \
    case OP_SIN:                    \
        fprintf(stream, " sin ");   \
        break;

#define PUT_COS                     \
    case OP_COS:                    \
        fprintf(stream, " cos ");   \
        break;

#define PUT_LN                      \
    case OP_LN:                     \
        fprintf(stream, " ln ");    \
        break;

#define PUT_POW                      \
    case OP_POW:                     \
        fprintf(stream, " ^ ");      \
        break;
#endif