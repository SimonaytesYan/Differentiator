#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <math.h>

#include "Differentiator.h"
#include "Libs/Bundles.h"

//#define DEBUG

//--------------------CONST AND STATIC VARIABLES--------------------

static FILE* LatexFp = nullptr;

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

static void  PrintRandBundles(FILE* stream);

static void  RemoveNeutralElem(Node* node);

static void  RemoveNeutralPlus(Node* node);

static void RemoveNeutralSub(Node* node);

static void RemoveNeutralMul(Node* node);

static void RemoveNeutralDiv(Node* node);

static void RemoveNeutralPow(Node* node);

static int  GetOpRank(OPER_TYPES operation);

//--------------------DSL--------------------

#define ReturnAndTex                        \
    PrintRandBundles(LatexFp);              \
                                            \
    PrintfInLatex("\\begin{center}\n""(");  \
    TexNode(node);                          \
    PrintfInLatex(")`\n");                  \
                                            \
    PrintfInLatex( " = ");                  \
    TexNode(new_node);                      \
    PrintfInLatex("\\end{center}\n");       \
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

#define CpyAndReplace(from_cpy, replace_it)     \
{                                               \
    PrintRandBundles(LatexFp);                  \
    PrintfInLatex("\\begin{center}\n")          \
    TexNode(node);                              \
    Node* replacement = CpyNode(from_cpy);      \
                                                \
    DelLR(replace_it);                          \
    *replace_it = *replacement;                 \
    free(replacement);                          \
                                                \
    PrintfInLatex("=")                          \
    TexNode(replace_it);                        \
    PrintfInLatex("\\end{center}\n")            \
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
                case OP_LN:
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

static void PrintRandBundles(FILE* stream)
{
    fprintf(stream, "%s", BUNDLES[rand() % BUNDLES_NUMBER]);
}

static Node* NodeCtorNum(double val)
{
    Node* new_node = (Node*)calloc(1, sizeof(Node));

    Node_t node_val  = {};
    node_val.type    = TYPE_NUM;
    node_val.val.dbl = val;

    NodeCtor(new_node, node_val);

    return new_node;
}

static Node* NodeCtorVar(char* val)
{
    Node* new_node = (Node*)calloc(1, sizeof(Node));

    Node_t node_val  = {};
    node_val.type    = TYPE_VAR;
    node_val.val.var = val;

    NodeCtor(new_node, node_val);

    return new_node;
}

static Node* NodeCtorOp(OPER_TYPES val)
{
    Node* new_node = (Node*)calloc(1, sizeof(Node));

    Node_t node_val  = {};
    node_val.type    = TYPE_OP;
    node_val.val.op  = val;

    NodeCtor(new_node, node_val);

    return new_node;
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

static Node* DiffLn(Node* node)
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
    R(RR(new_node))  = NodeCtorOp(OP_LN);
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
            case OP_LN:
                return DiffLn(node);
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

Node* CpyNode(Node* node)
{
    if (node == nullptr)
        return nullptr;
    
    #ifdef DEBUG
        PrintElem(stdout, node);
        printf(" node = %p\nleft = %p\nright = %p\n", node, L(node), R(node));
    #endif

    Node* new_node = (Node*)calloc(1, sizeof(Node));

    new_node->val.type = node->val.type;
    new_node->val.val  = node->val.val;
    if (TYPE(new_node) == TYPE_VAR)
    {
        VAL_VAR(new_node) = (char*)calloc(1, strlen(VAL_VAR(node)) + 1);
        strcpy(VAL_VAR(new_node), VAL_VAR(node));
    }

    L(new_node) = CpyNode(L(node));
    R(new_node) = CpyNode(R(node));

    return new_node;
}

int SaveTreeInFile(Tree* tree, const char file_name[])
{
    ReturnIfError(TreeCheck(tree));

    FILE* fp = fopen(file_name, "w");
    CHECK(fp == nullptr, "Error during open file", -1);

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

    fclose(fp);

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
        switch(VAL_OP(elem))
        {
            PUT_PLUS
            PUT_MUL
            PUT_SUB
            PUT_DIV
            PUT_SIN
            PUT_COS
            PUT_LN
            PUT_POW

            case UNDEF_OPER_TYPE:
                fprintf(stream, "?");
                break;
            default:
                fprintf(stream, "#");
                break;
        }
        break;
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
                  "\\begin{document}\n"
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

static void PreFuncTexNode(Node* node, void* dfs_fp)
{
    FILE* stream = (FILE*)dfs_fp;
                            
    if (IS_OP(node) && VAL_OP(node) == OP_DIV)
        fprintf(stream, "\\frac{");
    else if (!IS_NUM(node) && !IS_VAR(node))
    {
        if (VAL_OP(node) != OP_SIN && VAL_OP(node) != OP_COS && 
            VAL_OP(node) != OP_LN && VAL_OP(node) != OP_POW)
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

            case OP_LN:
                fprintf(stream, "ln(");
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

int TexNode(Node* root)
{
    assert(root);
    assert(LatexFp || "Latex file didnt open");

    #ifdef DEBUG
        printf("Start tex\n");    
    #endif

    PrintfInLatex("$");

    DFS(root, PreFuncTexNode,   LatexFp,
              PrintElemInLatex, LatexFp,
              PostFuncTexNode,  LatexFp);

    PrintfInLatex("$");

    fflush(LatexFp);

    return 0;
}

static void GetNodeValFromStr(const char str[], Node_t* val)
{
    assert(val);

    if (strcmp(str, "+") == 0)
    {
        #ifdef DEBUG
            printf("plus\n");
        #endif

        val->type   = TYPE_OP;
        val->val.op = OP_PLUS; 
    }
    else if (strcmp(str, "-") == 0)
    {
        #ifdef DEBUG
            printf("minus\n");
        #endif
        val->type   = TYPE_OP;
        val->val.op = OP_SUB;
    }
    else if (strcmp(str, "*") == 0)
    {
        #ifdef DEBUG
            printf("mul\n");
        #endif

        val->type   = TYPE_OP;
        val->val.op = OP_MUL;
    }
    else if (strcmp(str, "/") == 0)
    {
        #ifdef DEBUG
            printf("div\n");
        #endif

        val->type   = TYPE_OP;
        val->val.op = OP_DIV;
    }
    else if (strcmp(str, "sin") == 0)
    {
        #ifdef DEBUG
            printf("sin\n");
        #endif

        val->type   = TYPE_OP;
        val->val.op = OP_SIN;
    }
    else if (strcmp(str, "cos") == 0)
    {
        #ifdef DEBUG
            printf("cos\n");
        #endif

        val->type   = TYPE_OP;
        val->val.op = OP_COS;
    }
    else if (strcmp(str, "ln") == 0)
    {

        val->type   = TYPE_OP;
        val->val.op = OP_LN; 
    }
    else if (strcmp(str, "^") == 0)
    {
        val->type   = TYPE_OP;
        val->val.op = OP_POW;
    }
    else if (atof(str) == 0 && strcmp(str, "0"))
    {
        #ifdef DEBUG
            printf("var\n");
        #endif

        val->type    = TYPE_VAR;
        val->val.var = (char*)calloc(1, strlen(str) + 1);

        assert(val->val.var);

        strcpy(val->val.var, str);
    }
    else
    {
        val->type    = TYPE_NUM;
        val->val.dbl = atof(str);
        
        #ifdef DEBUG
            printf("num %lg\n", val->val.dbl);
        #endif
    }
}

void GetNodeFromFile(Node** node, FILE* fp)
{
    assert(node);
    assert(fp);

    int c = 0;
    while((c = getc(fp)) == ' ' || c == '\n' || c == '\t')
    {};

    #ifdef DEBUG
        printf("c = <%c>%d\n", c, c);
    #endif

    if (c == ')')
    {
        *node = nullptr;

        #ifdef DEBUG
            printf("end node\n");
        #endif

        return;
    }
    else if (c == '(')
    {
        #ifdef DEBUG
            printf("new node\n");
        #endif

        Node* new_node = (Node*)calloc(sizeof(Node), 1);

        assert(new_node);

        new_node->left = nullptr;
        new_node->right = nullptr;

        #ifdef DEBUG
            printf("Go to left\n");
        #endif

        GetNodeFromFile(&(new_node->left), fp);
        #ifdef DEBUG
            printf("End left\n");
        #endif
        
        while((c = fgetc(fp)) == ')' || c == '(' || c == ' ' || c == '\n' || c == '\t')
        {};
        ungetc(c, fp);

        char new_object[MAX_STR_LEN] = "";
        fscanf(fp, "%[^()\t\n ]", new_object);

        #ifdef DEBUG
            printf("\nnew_object = <%s>\n", new_object);
        #endif

        GetNodeValFromStr(new_object, &(new_node)->val);

        if (IS_OP(new_node) && 
           (VAL_OP(new_node) == OP_SIN || VAL_OP(new_node) == OP_COS || VAL_OP(new_node) == OP_LN))
        {
            DFSNodeDtor(L(new_node));
            L(new_node) = nullptr;
        }
        #ifdef DEBUG
            printf("\nGo to right\n");
        #endif
        GetNodeFromFile(&(new_node->right), fp);
        #ifdef DEBUG
            printf("End right\n");
        #endif

        NodeDtor(*node);
        *node = new_node;
    }
    else
        ungetc(c, fp);
}

int GetTreeFromFile(Tree* tree, const char file_name[])
{
    assert(tree);

    ReturnIfError(TreeCheck(tree));

    FILE* fp = fopen(file_name, "r");
    CHECK(fp == nullptr, "Error during open file", -1);

    GetNodeFromFile(&(tree->root), fp);

    fclose(fp);

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

    PrintfInLatex("\\newpage \\textbf{\\LARGE Упростим получившееся выражение}\n\n");
    
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
    
    PrintRandBundles(LatexFp);

    PrintfInLatex("\\begin{center}\n");
    TexNode(node);
    PrintfInLatex(" = ");

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
    case OP_LN:
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
    
    TexNode(node);
    PrintfInLatex("\\end{center}\n");
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