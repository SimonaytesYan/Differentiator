#include <math.h>

#include "Simplifying.h"
#include "../LatexOutput/LatexOutput.h"

static void   CpyAndReplace(Node* from_cpy, Node* replace_it);
 
static void   RemoveNeutralElem(Node* node);
 
static void   RemoveNeutralPlus(Node* node);
 
static void   RemoveNeutralSub(Node* node);
 
static void   RemoveNeutralMul(Node* node);
 
static void   RemoveNeutralDiv(Node* node);
 
static void   RemoveNeutralPow(Node* node);
 
static int    GetLetterFromIndes(int index);
 
static void   TexNodeWithDesignationsDFS(Node* node, Node** Designations, int height);

static double AnalisNodeForDesignation(Node* node, Node** Designations, int height, double coeff,
                                       int   tree_size);

static void   PrintAllDesignations(Node** Designations);
 
static void   EnterNewDesignation(Node* Designations[], Node* new_designation);

static int    CountNodeNumber(Node* root);

static int CountNodeNumber(Node* node)
{
    if (node == nullptr)
        return 0;
    return CountNodeNumber(L(node)) + CountNodeNumber(R(node)) + 1;
}

static void   CpyAndReplace(Node* from_cpy, Node* replace_it)
{                                                           
    PrintRandBundleInLatex();  
    TexEqualityWithDesignations(replace_it, from_cpy,    
                                "\\begin{center}\n", "");
    PrintfInLatex("\\end{center}\n")                     
    
    Node* replacement = CpyNode(from_cpy);

    DelLR(replace_it);
    EquateNode(replace_it, replacement);
    NodeDtor(replacement);
}

void SimplifyNode(Node* node)
{
    assert(node);

    Node* old_node = nullptr;
    do
    {
        DeleteNode(old_node);
        old_node = CpyNode(node);

        ConstsConvolution(node);
        RemoveNeutralElem(node);
    }
    while(NodeCmp(old_node, node));

    DeleteNode(old_node);
}

void ConstsConvolution(Node* node)
{
    if (node == nullptr)
        return;

    ConstsConvolution(L(node));
    ConstsConvolution(R(node));

    if (TYPE(node) != TYPE_OP)
        return;

    if ((L(node) != nullptr && !IS_NUM(L(node))) || !IS_NUM(R(node)))
        return;

    #ifdef DEBUG
        printf("Start ConstsConvolution: %p\n", node);
    #endif
    
    PrintRandBundleInLatex();

    Node* old_node = CpyNode(node);

    switch (VAL_OP(node))
    {
    case OP_PLUS:
        BinaryConstConv(+);
        break;
    case OP_SUB:
        BinaryConstConv(-);
        break;
    case OP_MUL:
        BinaryConstConv(*);
        break;
    case OP_DIV:
    {
        if (IS_DOUBLE_EQ(VAL_N(R(node)), 0))
            break;
        BinaryConstConv(/);
        break;
    }
    case OP_SIN:
        UnaryConstConv(sin);
        break;
    case OP_COS:
        UnaryConstConv(cos);
        break;
    case OP_LOG:
    {
        if (IS_DOUBLE_EQ(VAL_N(R(node)), 0))
            break;
        UnaryConstConv(log);
        break;
    }
    case OP_POW:
    {
        double a = VAL_N(L(node));
        double b = VAL_N(R(node));

        DelLR(node);

        TYPE(node)  = TYPE_NUM;
        VAL_N(node) = pow(a, b);
        break;
    }
    case UNDEF_OPER_TYPE:
        assert(1 || "undef operator");
        break;
    default:
        fprintf(stderr, "operation type = %d\n", VAL_OP(node));
        assert(1 || "unknown operator");
        break;
    }
    
    TexEqualityWithDesignations(old_node, node, "\\begin{center}", "");
    PrintfInLatex("\\end{center}\n");

    DeleteNode(old_node);
}

static void RemoveNeutralPlus(Node* node)
{
    assert(node);

    if (IS_ZERO(L(node)))
        CpyAndReplace(R(node), node);
    else if (IS_ZERO(R(node)))
        CpyAndReplace(L(node), node);
}

static void RemoveNeutralSub(Node* node)
{
    assert(node);

    if (IS_ZERO(R(node)))
        CpyAndReplace(L(node), node);
}

static void RemoveNeutralMul(Node* node)
{
    assert(node);

    if (IS_ZERO(L(node)) || IS_ZERO(R(node)))
    {
        Node* new_node = NodeCtorNum(0);
        CpyAndReplace(new_node, node);
        NodeDtor(new_node);
    }
    else if (IS_ONE(L(node))) 
        CpyAndReplace(R(node), node);
    else if (IS_ONE(R(node)))
        CpyAndReplace(L(node), node);
}

static void RemoveNeutralDiv(Node* node)
{
    assert(node);

    if (IS_ONE(R(node)))
        CpyAndReplace(L(node), node);
    else if (IS_ZERO(L(node)))
    {
        Node* new_node = NodeCtorNum(0);
        CpyAndReplace(new_node, node);
        NodeDtor(new_node);
    }
}

static void RemoveNeutralPow(Node* node)
{
    assert(node);
    
    //! pow(0,0) = 1. Because I decided so
    if (IS_ONE(L(node)))
    {
        Node* new_node = NodeCtorNum(1);
        CpyAndReplace(new_node, node);
        NodeDtor(new_node);
    }
    else if (IS_ZERO(L(node)))
    {
        Node* new_node = NodeCtorNum(0);
        CpyAndReplace(new_node, node);
        NodeDtor(new_node);
    }
    else if (IS_ONE(R(node)))
        CpyAndReplace(L(node), node);
    else if (IS_ZERO(R(node)))
    {
        Node* new_node = NodeCtorNum(1);
        CpyAndReplace(new_node, node);
        NodeDtor(new_node);
    }
}

static void RemoveNeutralElem(Node* node)
{
    if (node == nullptr || L(node) == nullptr || R(node) == nullptr)
        return;

    RemoveNeutralElem(L(node));
    RemoveNeutralElem(R(node));

    switch(VAL_OP(node))
    {
        case OP_PLUS:
            RemoveNeutralPlus(node);
            break;
        case OP_SUB:
            RemoveNeutralSub(node);            
            break;
        case OP_MUL:
            RemoveNeutralMul(node);
            break;
        case OP_DIV:
            RemoveNeutralDiv(node);
            break;
        case OP_POW:
            RemoveNeutralPow(node);
            break;
        case OP_COS:
        case OP_SIN:
        case UNDEF_OPER_TYPE:
        case OP_LOG:
        default:
            break;
    }
}

static void EnterNewDesignation(Node* Designations[], Node* new_designation)
{
    assert(Designations);

    for(int i = 0; i < MAX_DIS_NUM; i++)
    {
        if (NodeCmp(Designations[i], new_designation) == 0)
            return;
        if (Designations[i] == nullptr)
        {
            #ifdef DEBUG
                printf("Add new designation\n");
            #endif
            Designations[i] = new_designation;
            return;
        }
    }
    assert(0 && "Imposible add new Designation, because all elements in Designations arn`t empty");
}

void TexEqualityWithDesignations(Node* node_a, Node* node_b, const char pre_decoration[],
                                                             const char in_decoration[])
{
    assert(node_a);
    assert(node_b);

    Node* Designations[MAX_DIS_NUM] = {};
    AnalisNodeForDesignation(node_a, Designations, 0, 1, CountNodeNumber(node_a)); 
    AnalisNodeForDesignation(node_b, Designations, 0, 1, CountNodeNumber(node_b));

    PrintAllDesignations(Designations);

    PrintfInLatex(pre_decoration);
    PrintfInLatex("$");
    TexNodeWithDesignationsDFS(node_a, Designations, 0);

    PrintfInLatex(in_decoration);
    PrintfInLatex(" = ");

    TexNodeWithDesignationsDFS(node_b, Designations, 0);
    PrintfInLatex("$");
}

static void PrintAllDesignations(Node** Designations)
{
    assert(Designations);

    for(int i = 0; i < MAX_DIS_NUM; i++)
    {
        if (Designations[i] != nullptr)
        {
            PrintfInLatex("\\begin{center}\n" 
                          "%c = ", GetLetterFromIndes(i));
            PrintfInLatex("$");
            TexNodeWithDesignationsDFS(Designations[i], Designations, 0);
            PrintfInLatex("$");
            PrintfInLatex("\\end{center}\n");
        }
    }
}

static bool ShouldEnternewDesignation(int nnodes_in_subtree, int tree_size, int height)
{
    return (nnodes_in_subtree > tree_size/THRESHOLD_ENTER_DESIGNATION && 
            nnodes_in_subtree > THRESHOLD_ENTER_DESIGNATION           && 
            height != 0);
}

//!------------------------------
//!
//!@param [in] node node for analis   
//!@return          number of elements in subtree of given node
//!-------------------------------

static double AnalisNodeForDesignation(Node* node, Node** Designations, int height, double coeff,
                                       int tree_size)
{
    if (node == nullptr)
        return 0;

    if (IS_OP(node) && VAL_OP(node) == OP_DIV)
        coeff /= 2;

    double nnodes_in_subtree = 0;
    nnodes_in_subtree += AnalisNodeForDesignation(L(node), Designations, height + 1, coeff, tree_size);

    if (IS_OP(node) && VAL_OP(node) == OP_POW)
        coeff /= 2;
    nnodes_in_subtree += AnalisNodeForDesignation(R(node), Designations, height + 1, coeff, tree_size);
    
    nnodes_in_subtree += coeff;

    if (ShouldEnternewDesignation(nnodes_in_subtree, tree_size, height))
    {
        EnterNewDesignation(Designations, node);
        nnodes_in_subtree = -2*THRESHOLD_ENTER_DESIGNATION;        
    }

    return nnodes_in_subtree;
}

static int GetLetterFromIndes(int index)
{
    int letter = index + 'A';
    if (letter > 90)
        letter+=6;
    return letter;
}

void TexNodeWithDesignations(Node* root, const char pre_decoration[])
{
    assert(root);
    Node* Designations[MAX_DIS_NUM] = {0};

    #ifdef DEBUG
        printf("Start tex node with Designations\n");
    #endif

    AnalisNodeForDesignation(root, Designations, 0, 1, CountNodeNumber(root));
    PrintAllDesignations(Designations);

    PrintfInLatex(pre_decoration);

    PrintfInLatex("$");
    TexNodeWithDesignationsDFS(root, Designations, 0);
    PrintfInLatex("$\n\n");
}

void TexNodeWithDesignationsDFS(Node* node, Node** Designations, int height)
{
    if (node == nullptr)
        return;
    assert(Designations);

    if (height != 0)
    {
        for(int i = 0; i < MAX_DIS_NUM; i++)
        {
            if (NodeCmp(node, Designations[i]) == 0)
            {
                PrintfInLatex("%c", GetLetterFromIndes(i));
                return;
            }
        }
    }

    PreFuncTexNode(node, nullptr);

    TexNodeWithDesignationsDFS(L(node), Designations, height + 1);
    
    PrintElemInLatex(node, nullptr);

    TexNodeWithDesignationsDFS(R(node), Designations, height + 1);

    PostFuncTexNode(node, nullptr);
}
