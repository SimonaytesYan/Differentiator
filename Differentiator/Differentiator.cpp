#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <math.h>

#include "Differentiator.h"
#include "Libs/Bundles.h"
#include "Libs/DSL.h"
#include "Libs/ResursiveDescent/RecursiveDescent.h"

//#define DEBUG

FILE* LatexFp = nullptr;

//--------------------FUNCTION PROTOTIPES--------------------

static void  GetNodeValFromStr(const char str[], Node_t* val);

static void  PrintElem(FILE* stream, Node* elem);

static void  PrintfInLatexEndDoc();

static void  PrintInLatexStartDoc();

static Node* NodeCtorNum(double val);

static Node* NodeCtorVar(char* val);

static Node* NodeCtorOp(OPER_TYPES val);

static Node* DiffDiv(Node* node_arg);

static Node* DiffMult(Node* node_arg);

static Node* DiffSin(Node* node_arg);

static Node* DiffCos(Node* node_arg);

static Node* DiffSum(Node* node_arg);

static Node* DiffSub(Node* node_arg);

static void  PostFuncTexNode(Node* node, void* dfs_fp);

static void  PreFuncTexNode(Node* node, void* dfs_fp);

static void  RemoveNeutralElem(Node* node);

static void  RemoveNeutralPlus(Node* node);

static void  RemoveNeutralSub(Node* node);

static void  RemoveNeutralMul(Node* node);

static void  RemoveNeutralDiv(Node* node);

static void  RemoveNeutralPow(Node* node);

static int   GetOpRank(OPER_TYPES operation);

static int   GetLetterFromIndes(int index);

static void  PrintElemInLatex(Node* node, void* dfs_fp);

static void  TexNodeWithDesignationsDFS(Node* node, Node** Designations, int height);

static int AnalisNodeForDesignation(Node* node, Node** Designations, int height);

static void PrintAllDesignations(Node** Designations);

static void TexEqualityWithDesignations(Node* node_a, Node* node_b, const char pre_decoration[],
                                                             const char in_decoration[]);

//--------------------FUNCTION IMPLEMENTATION--------------------

#define PrintfInLatex(format, ...) PrintfInLatexReal(__PRETTY_FUNCTION__, format,##__VA_ARGS__);

void PrintfInLatexReal(const char* function, const char *format, ...)
{
    if (LatexFp == nullptr)
    {
        fprintf(stderr, "%s\n", function);
        assert(LatexFp == nullptr);
    }

    va_list args;
    va_start(args, format);

    vfprintf(LatexFp, format, args);

    va_end(args);
    fflush(LatexFp);
}

static int  GetOpRank(Node* node)
{
    switch (TYPE(node))
    {
        case TYPE_OP:
            switch (VAL_OP(node))
            {
                case OP_POW:
                case OP_SIN:
                case OP_LOG:
                case OP_COS:
                    return 2;
                
                case OP_DIV:
                case OP_MUL:
                    return 1;
                
                case OP_PLUS:
                case OP_SUB:
                    return 0;  
                
                default:
                    return -1;
            }
        case TYPE_NUM:
        case TYPE_VAR:
            return 0;
        default:
            return -1;
    }
    
}

void PrintRandBundleInLatex()
{
    PrintfInLatex("%s", BUNDLES[rand() % BUNDLES_NUMBER]);
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

    ReturnAndTex;
}

static Node* DiffMult(Node* node)
{
    Node* new_node   = NodeCtorOp(OP_PLUS);
    Node* left_node  = NodeCtorOp(OP_MUL);
    Node* right_node = NodeCtorOp(OP_MUL);

    L(new_node) = left_node;
    R(new_node) = right_node;

    LL(new_node) = Diff(L(node));
    LR(new_node) = CpyNode(R(node));

    RL(new_node) = CpyNode(L(node));
    RR(new_node) = Diff(R(node));

    ReturnAndTex;
}

static Node* DiffSin(Node* node)
{
    Node* new_node = NodeCtorOp(OP_MUL);

    L(new_node) = Diff(R(node));
    R(new_node) = NodeCtorOp(OP_COS);

    RL(new_node) = nullptr;
    RR(new_node) = CpyNode(R(node));

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

    ReturnAndTex;
}

static Node* DiffSum(Node* node)
{
    Node* new_node  = NodeCtorOp(OP_PLUS);

    new_node->left  = Diff(L(node));
    new_node->right = Diff(R(node));

    ReturnAndTex;
}

static Node* DiffSub(Node* node)
{
    Node* new_node  = NodeCtorOp(OP_SUB);

    L(new_node) = Diff(L(node));
    R(new_node) = Diff(R(node));
                
    ReturnAndTex;
}

static Node* DiffLog(Node* node)
{
    Node* new_node = NodeCtorOp(OP_MUL);

    L(new_node)  = NodeCtorOp(OP_DIV);
    LL(new_node) = NodeCtorNum(1);

    LR(new_node) = CpyNode(R(node));
    R(new_node)  = Diff(R(node));

    ReturnAndTex;
}

static Node* DiffPow(Node* node)
{
    Node* new_node = NodeCtorOp(OP_MUL);

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
                return DiffCos(node);
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

int SaveTreeInFile(Tree* tree, FILE* fp)
{
    ReturnIfError(TreeCheck(tree));
    CHECK(fp == nullptr, "fp = nullptr", -1);

    DFS_f pre_function = [](Node*, void* dfs_fp)
                       {
                            fprintf((FILE*)dfs_fp, "(");
                       };

    DFS_f in_function  = [](Node* node, void* dfs_fp)
                        {
                            PrintElem((FILE*)dfs_fp, node);
                        };

    DFS_f post_function = [](Node*, void* dfs_fp)
                        {
                            fprintf((FILE*)dfs_fp, ")");
                        };

    DFS(tree->root, pre_function,  fp,
                    in_function,   fp,
                    post_function, fp);

    fflush(fp);

    return 0;
}

static void PrintElem(FILE* stream, Node* elem)
{
    switch (TYPE(elem))
    {
    case TYPE_VAR:  
        fprintf(stream, "%s", VAL_VAR(elem));
        break;
    case TYPE_OP:
    {
        switch(VAL_OP(elem))
        {
            PUT_PLUS
            PUT_MUL
            PUT_SUB
            PUT_DIV
            PUT_SIN
            PUT_COS
            PUT_LOG
            PUT_POW

            case UNDEF_OPER_TYPE:
                fprintf(stream, "?");
                break;
            default:
                fprintf(stream, "#");
                break;
        }
        break;
    }
    case TYPE_NUM:
        fprintf(stream, "%lg", VAL_N(elem));
        break;
    case UNDEF_NODE_TYPE:
        fprintf(stream, "\n");
        break;
    default:
        fprintf(stream, "\t");
        break;
    }
}

void PrintElemInLog(Node_t elem)
{
    LogPrintf("type = %d\n" "{\n", elem.type);
    LogPrintf("\tdbl  = %lg\n", elem.val.dbl);
    LogPrintf("\top   = %d\n", elem.val.op);
    LogPrintf("\tvar  = <%s?\n", elem.val.var);
    LogPrintf("}\n");
}

static void PrintInLatexStartDoc()
{
    PrintfInLatex("\\documentclass[12pt,a4paper,fleqn]{article}\n"
                  "\\usepackage[utf8]{inputenc}\n"
                  "\\usepackage[russian]{babel}\n"
                  "\\usepackage{amssymb, amsmath, multicol}\n"
                  "\\usepackage{enumitem}\n"
                  "\\usepackage{lipsum}\n"
                  "\\usepackage{euler}\n"
                  "\\oddsidemargin=-15.4mm\n"
                  "\\textwidth=190mm\n"
                  "\\headheight=-32.4mm\n"
                  "\\textheight=277mm\n"
                  "\\parindent=0pt\n"
                  "\\parskip=8pt\n"
                  "\\pagestyle{empty}\n"
                  "\\usepackage{graphicx}\n"

                  "\\begin{document}\n"
                  "\\begin{center}\n"
                  "\\textbf{\\LARGE{Исследовательская работа по теме:\\\\"
                  "Исследование функции дифференциальными методами}}"
                  "\\end{center}"
                  "\\newpage"
                 );
}

static void PrintfInLatexEndDoc()
{
    PrintfInLatex("\n"
                  "\\end{document}");
}

int OpenLatexFile(const char file_name[])
{
    LatexFp = fopen(file_name, "w");
    CHECK(LatexFp == nullptr, "Error during open file", -1);
    
    PrintInLatexStartDoc();

    return 0;
}

void CloseLatexFile()
{
    PrintfInLatexEndDoc();

    fclose(LatexFp);
    LatexFp = nullptr;
}

void EnterNewDesignation(Node* Designations[], Node* new_designation)
{
    assert(Designations);

    for(int i = 0; i < MAX_DIS_NUM; i++)
    {
        if (NodeCmp(Designations[i], new_designation) == 0)
            return;
        if (Designations[i] == nullptr)
        {
            printf("Add new designation\n");

            Designations[i] = new_designation;
            return;
        }
    }
    assert(0 && "Imposible add new Designation, because all elements in Designations arn`t empty");
}

void DtorDesignations(Node** Designations)
{
    assert(Designations);

    for(int i = 0; i < MAX_DIS_NUM; i++)
    {
        if (Designations[i] != nullptr)
            DeleteNode(Designations[i]);
    }
}

void TexEqualityWithDesignations(Node* node_a, Node* node_b, const char pre_decoration[],
                                                             const char in_decoration[])
{
    assert(node_a);
    assert(node_b);

    Node* Designations[MAX_DIS_NUM] = {};
    AnalisNodeForDesignation(node_a, Designations, 0);
    AnalisNodeForDesignation(node_b, Designations, 0);

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

//!------------------------------
//!
//!@param [in] node node for analis   
//!@return          number of elements in subtree of given node
//!-------------------------------

static int AnalisNodeForDesignation(Node* node, Node** Designations, int height)
{
    if (node == nullptr)
        return 0;

    int nnodes_in_subtree = 0;
    nnodes_in_subtree += AnalisNodeForDesignation(L(node), Designations, height + 1);
    nnodes_in_subtree += AnalisNodeForDesignation(R(node), Designations, height + 1);
    
    nnodes_in_subtree++;

    if (nnodes_in_subtree > THRESHOLD_ENTER_DESIGNATION && height != 0)
    {
        EnterNewDesignation(Designations, node);
        nnodes_in_subtree = 1;        
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

    AnalisNodeForDesignation(root, Designations, 0);
    PrintAllDesignations(Designations);

    PrintfInLatex(pre_decoration);

    PrintfInLatex("$");
    TexNodeWithDesignationsDFS(root, Designations, 0);
    PrintfInLatex("$\n\n");

    fflush(LatexFp);
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

    PreFuncTexNode(node, LatexFp);

    TexNodeWithDesignationsDFS(L(node), Designations, height + 1);
    
    PrintElemInLatex(node, LatexFp);

    TexNodeWithDesignationsDFS(R(node), Designations, height + 1);

    PostFuncTexNode(node, LatexFp);
}


static void PreFuncTexNode(Node* node, void* dfs_fp)
{
    FILE* stream = (FILE*)dfs_fp;
                            
    if (IS_OP(node) && VAL_OP(node) == OP_DIV)
        fprintf(stream, "\\frac{");
    else if (!IS_NUM(node) && !IS_VAR(node))
    {
        if (VAL_OP(node) != OP_SIN && VAL_OP(node) != OP_COS && 
            VAL_OP(node) != OP_LOG && VAL_OP(node) != OP_POW)
            fprintf(stream, "(");
    }
}

static void PrintElemInLatex(Node* node, void* dfs_fp)
{
    assert(node);

    FILE* stream = (FILE*)dfs_fp;

    switch (TYPE(node))
    {
    case TYPE_VAR:
        fprintf(stream, "%s", VAL_VAR(node));
        break;
    case TYPE_OP:
        switch(VAL_OP(node))
        {
            PUT_PLUS
            PUT_SUB

            case OP_SIN:
                fprintf(stream, "sin(");
                break;
            
            case OP_COS:
                fprintf(stream, "cos(");
                break;

            case OP_LOG:
                fprintf(stream, "log(");
                break;

            case OP_POW:
                fprintf(stream, "^{");
                break;

            case OP_MUL:
                fprintf(stream, " \\cdot ");
                break;

            case OP_DIV:
                fprintf(stream, "}{");
                break;
            
            case UNDEF_OPER_TYPE:
                fprintf(stream, "?");
                break;

            default:
                fprintf(stream, "#");
                break;
        }
        break;
    case TYPE_NUM:
        fprintf(stream, "%lg", VAL_N(node));
        break;
    case UNDEF_NODE_TYPE:
        fprintf(stream, "\n");
        break;
    default:
        fprintf(stream, "\t");
        break;
    }
}

static void PostFuncTexNode(Node* node, void* dfs_fp)
{
    FILE* stream = (FILE*)dfs_fp;

    if (IS_OP(node) && (VAL_OP(node) == OP_DIV || VAL_OP(node) == OP_POW))
        fprintf(stream, "}");
    else if (!IS_NUM(node) && !IS_VAR(node))
        fprintf(stream, ")");
}

void TexNode(Node* root)
{
    assert(root);

    #ifdef DEBUG
        printf("Start tex node\n");    
    #endif

    PrintfInLatex("$");

    DFS(root, PreFuncTexNode,   LatexFp,
              PrintElemInLatex, LatexFp,
              PostFuncTexNode,  LatexFp);

    PrintfInLatex("$");

    fflush(LatexFp);
}

int GetTreeFromFile(Tree* tree, const char file_name[])
{
    assert(tree);

    ReturnIfError(TreeCheck(tree));

    FILE* fp = fopen(file_name, "r");
    CHECK(fp == nullptr, "Error during open file", -1);

    char function[101] = "";
    fgets(function, 100, fp);
    fclose(fp);

    printf("function = <%s>\n", function);

    tree->root = GetNodeFromStr(function);

    return 0;
}

void OutputGraphicDump(Tree* tree)
{
    assert(tree);

    GraphicDump(tree);

    char graphic_dump_file[70] = "";
    #ifdef _WIN32
        sprintf(graphic_dump_file, ".\\GraphicDumps\\Dump%d.png", GRAPHIC_DUMP_CNT - 1);
    #else
        sprintf(graphic_dump_file, "xdg-open GraphicDumps/Dump%d.png", GRAPHIC_DUMP_CNT - 1);
    #endif

    system(graphic_dump_file);
}

void SimplifyTree(Tree* tree)
{
    assert(tree);

    Node* old_tree = nullptr;
    do
    {
        DeleteNode(old_tree);
        old_tree = CpyNode(tree->root);

        ConstsConvolution(tree->root);
        RemoveNeutralElem(tree->root);
    }
    while(NodeCmp(old_tree, tree->root));

    DeleteNode(old_tree);
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

    PrintfInLatex("\\begin{center}\n");
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
        BinaryConstConv(/);
        break;
    case OP_SIN:
        UnaryConstConv(sin);
        break;
    case OP_COS:
        UnaryConstConv(cos);
        break;
    case OP_LOG:
        UnaryConstConv(log);
        break;
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
        CpyAndReplace(R(node), node)
    else if (IS_ZERO(R(node)))
        CpyAndReplace(L(node), node)
}

static void RemoveNeutralSub(Node* node)
{
    assert(node);

    if (IS_ZERO(R(node)))
        CpyAndReplace(L(node), node)    
}

static void RemoveNeutralMul(Node* node)
{
    assert(node);

    if (IS_ZERO(L(node)) || IS_ZERO(R(node)))
    {
        Node* new_node = NodeCtorNum(0);
        CpyAndReplace(new_node, node);
        free(new_node);
    }
    else if (IS_ONE(L(node))) 
        CpyAndReplace(R(node), node)
    else if (IS_ONE(R(node)))
        CpyAndReplace(L(node), node)
}

static void RemoveNeutralDiv(Node* node)
{
    assert(node);

    if (IS_ONE(R(node)))
        CpyAndReplace(L(node), node)
    else if (IS_ZERO(L(node)))
    {
        Node* new_node = NodeCtorNum(0);
        CpyAndReplace(new_node, node);
        free(new_node);
    }
}

static void RemoveNeutralPow(Node* node)
{
    assert(node);
    
    if (IS_ONE(L(node)))
    {
        Node* new_node = NodeCtorNum(1);
        CpyAndReplace(new_node, node);
        free(new_node);
    }
    else if (IS_ZERO(L(node)))
    {
        Node* new_node = NodeCtorNum(0);
        CpyAndReplace(new_node, node);
        free(new_node);
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
        default:
            break;
    }
}