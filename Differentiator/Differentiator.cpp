#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

#include "Differentiator.h"

//#define DEBUG

//--------------------STATIC VARIABLES--------------------

static FILE* LatexFp = nullptr;

//--------------------FUNCTION PROTOTIPES--------------------

static void  GetNodeValFromStr(const char str[], Node_t* val);

static void  PrintElem(FILE* stream, Node_t elem);

static void  PrintfInLatexEndDoc(FILE* fp);

static void  PrintInLatexStartDoc(FILE* fp);

static Node* NodeCtorNum(double val);

static Node* NodeCtorVar(char* val);

static Node* NodeCtorOp(OPER_TYPES val);

static Node* DiffDiv(Node* node_arg);

static Node* DiffMult(Node* node_arg);

static Node* DiffSin(Node* node_arg);

static Node* DiffCos(Node* node_arg);

static Node* DiffSum(Node* node_arg);

static Node* DiffSub(Node* node_arg);

//--------------------DSL--------------------

#define L(node) node->left

#define R(node) node->right

#define LL(node) node->left->left

#define LR(node) node->left->right

#define RL(node) node->right->left

#define RR(node) node->right->right


#define IS_VAR(node) (node.type == TYPE_VAR)

#define IS_OP(node) (node.type == TYPE_OP)

#define IS_NUM(node) (node.type == TYPE_NUM)


#define VAL_N(node) node.val.dbl

#define VAL_OP(node) node.val.op

#define VAL_VAR(node) node.val.var


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


#define ReturnAndTex                \
    PrintfInLatex("(");             \
    TexNode(node_arg);              \
    PrintfInLatex(")`");            \
                                    \
    PrintfInLatex( " = ");          \
    TexNode(new_node);              \
    PrintfInLatex("\\\\\n");        \
    return new_node;

//--------------------FUNCTION IMPLEMENTATION--------------------

#define PrintfInLatex(format, ...) PrintfInLatexReal(__PRETTY_FUNCTION__, format,##__VA_ARGS__);

void PrintfInLatexReal(const char* function, const char *format, ...)
{
    if (LatexFp == nullptr)
    {
        printf("%s\n", function);
        assert(LatexFp == nullptr);
    }

    va_list args;
    va_start(args, format);

    vfprintf(LatexFp, format, args);

    va_end(args);
    fflush(LatexFp);
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

static Node* DiffDiv(Node* node_arg)
{
    Node* new_node = NodeCtorOp(OP_DIV);

    L(new_node) = NodeCtorOp(OP_SUB);
    R(new_node) = NodeCtorOp(OP_MUL);

    LL(new_node) = NodeCtorOp(OP_MUL); 
    LR(new_node) = NodeCtorOp(OP_MUL);

    L(LL(new_node)) = Diff(L(node_arg));
    R(LL(new_node)) = Cpy(R(node_arg));
            
    L(LR(new_node)) = Diff(R(node_arg));
    R(LR(new_node)) = Cpy(L(node_arg));

    RR(new_node) = Cpy(R(node_arg));
    RL(new_node) = Cpy(R(node_arg));

    ReturnAndTex;
}

static Node* DiffMult(Node* node_arg)
{
    Node* new_node   = NodeCtorOp(OP_PLUS);
    Node* left_node  = NodeCtorOp(OP_MUL);
    Node* right_node = NodeCtorOp(OP_MUL);

    L(new_node) = left_node;
    R(new_node) = right_node;

    LL(new_node) = Diff(L(node_arg));
    LR(new_node) = Cpy(R(node_arg));

    RL(new_node) = Cpy(L(node_arg));
    RR(new_node) = Diff(R(node_arg));

    ReturnAndTex;
}

static Node* DiffSin(Node* node_arg)
{
    Node* new_node = NodeCtorOp(OP_MUL);

    L(new_node) = Diff(R(node_arg));
    R(new_node) = NodeCtorOp(OP_COS);

    RL(new_node) = nullptr;
    RR(new_node) = Cpy(R(node_arg));

    ReturnAndTex;
}

static Node* DiffCos(Node* node_arg)
{
    Node* new_node = NodeCtorOp(OP_MUL);

    L(new_node)  = Diff(R(node_arg));
    R(new_node)  = NodeCtorOp(OP_SUB);

    RR(new_node) = NodeCtorOp(OP_SIN);
    RL(new_node) = NodeCtorNum(0);

    L(RR(new_node)) = nullptr;
    R(RR(new_node)) = Cpy(R(node_arg));

    ReturnAndTex;
}

static Node* DiffSum(Node* node_arg)
{
    Node* new_node  = NodeCtorOp(OP_PLUS);

    new_node->left  = Diff(node_arg->left);
    new_node->right = Diff(node_arg->right);

    ReturnAndTex;
}

static Node* DiffSub(Node* node_arg)
{
    Node* new_node  = NodeCtorOp(OP_SUB);

    new_node->left  = Diff(node_arg->left);
    new_node->right = Diff(node_arg->right);
                
    ReturnAndTex;
}

Node* Diff(Node* node_arg)
{
    assert(node_arg);
    Node_t node = node_arg->val;

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
                return DiffSum(node_arg);
            case OP_SUB:
                return DiffSub(node_arg);
            case OP_MUL:
                return DiffMult(node_arg);
            case OP_DIV:
                return DiffDiv(node_arg);
            case OP_SIN:
                return DiffSin(node_arg);
            case OP_COS:
                return DiffCos(node_arg);

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
              "Node type = %d\n", node.type);
    return nullptr;
}

Node* Cpy(Node* node)
{
    if (node == nullptr)
        return nullptr;
    PrintElem(stdout, node->val);
    
    #ifdef DEBUG
        printf(" node = %p\nleft = %p\nright = %p\n", node, L(node), R(node));
    #endif

    Node* new_node = (Node*)calloc(1, sizeof(Node));

    new_node->val.type = node->val.type;
    new_node->val.val  = node->val.val;

    new_node->left     = Cpy(node->left);
    new_node->right    = Cpy(node->right);

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
                            PrintElem((FILE*)dfs_fp, node->val);
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

static void PrintElem(FILE* stream, Node_t elem)
{
    switch (elem.type)
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
    LogPrintf("\tdbl  = %lg\n", VAL_N(elem));
    LogPrintf("\top   = %d\n", VAL_OP(elem));
    LogPrintf("\tvar  = <%s?\n", VAL_VAR(elem));
    LogPrintf("}\n");
}

static void PrintInLatexStartDoc(FILE* fp)
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

static void PrintfInLatexEndDoc(FILE* fp)
{
    PrintfInLatex("\n"
                  "\\end{document}");
}

static void PrintElemInLatex(Node* node, void* dfs_fp)
{
    FILE* stream = (FILE*)dfs_fp;
    Node_t val = node->val;

    switch (node->val.type)
    {
    case TYPE_VAR:  
        fprintf(stream, "%s", VAL_VAR(val));
        break;
    case TYPE_OP:
        switch(VAL_OP(val))
        {
            PUT_PLUS
            PUT_SUB

            case OP_SIN:
                fprintf(stream, "sin(");
                break;
            
            case OP_COS:
                fprintf(stream, "cos(");
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
        fprintf(stream, "%lg", VAL_N(val));
        break;
    case UNDEF_NODE_TYPE:
        fprintf(stream, "\n");
        break;
    default:
        fprintf(stream, "\t");
        break;
    }
}

int OpenLatexFile(const char file_name[])
{
    LatexFp = fopen(file_name, "w");
    CHECK(LatexFp == nullptr, "Error during open file", -1);
    
    PrintInLatexStartDoc(LatexFp);

    return 0;
}

void CloseLatexFile()
{
    PrintfInLatexEndDoc(LatexFp);

    fclose(LatexFp);
    LatexFp = nullptr;
}

int TexNode(Node* root)
{
    assert(root);
    assert(LatexFp || "Latex file didnt open");

    printf("Start tex\n");    

    PrintfInLatex( "$");

    DFS_f pre_function = [](Node* node, void* dfs_fp)
                       {
                            FILE* stream = (FILE*)dfs_fp;
                            Node_t val = node->val;
                            
                            if (IS_OP(val) && VAL_OP(val) == OP_DIV)
                                fprintf(stream, "\\frac{");
                            else if (!IS_NUM(val) && !IS_VAR(val))
                            {
                                if (VAL_OP(val) != OP_SIN && VAL_OP(val) != OP_COS)
                                    fprintf(stream, "(");
                            }
                       };

    DFS_f post_function = [](Node* node, void* dfs_fp)
                        {
                            FILE* stream = (FILE*)dfs_fp;
                            Node_t val = node->val;

                            if (IS_OP(val) && VAL_OP(val) == OP_DIV)
                                fprintf(stream, "}");
                            else if (!IS_NUM(val) && !IS_VAR(val))
                                fprintf(stream, ")");
                        };

    DFS(root, pre_function,     LatexFp,
              PrintElemInLatex, LatexFp,
              post_function,    LatexFp);

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
    else if (atof(str) == 0 && strcmp(str, "0"))
    {
        #ifdef DEBUG
            printf("var\n");
        #endif

        val->type    = TYPE_VAR;
    
        val->val.var = (char*)calloc(1, strlen(str));
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
        printf("end node\n");
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

        if (new_node->val.type == TYPE_OP && 
           (VAL_OP(new_node->val) == OP_SIN || VAL_OP(new_node->val) == OP_COS))
        {
            free(L(new_node));
            L(new_node) = nullptr;
        }
        #ifdef DEBUG
            printf("\nGo to right\n");
        #endif
        GetNodeFromFile(&(new_node->right), fp);
        #ifdef DEBUG
            printf("End right\n");
        #endif

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