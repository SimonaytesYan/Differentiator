#include <stdio.h>
#include <assert.h>

#include "Differentiator.h"

#define DEBUG

static void GetNodeValFromStr(const char str[], Node_t* val);

static void PrintElem(FILE* stream, Node_t elem);

static void PrintfInLatexEndDoc(FILE* fp);

static void PrintInLatexStartDoc(FILE* fp);

#define VAL_N(node) node.val.dbl

#define VAL_OP(node) node.val.op

#define VAL_VAR(node) node.val.var

#define PUT_PLUS                \
    case OP_PLUS:               \
        fprintf(stream, " + ");   \
        break;

#define PUT_SUB                \
    case OP_SUB:               \
        fprintf(stream, " - ");  \
        break;
        
#define PUT_MUL                 \
    case OP_MUL:                \
        fprintf(stream, " * ");   \
        break;
        
#define PUT_DIV                \
    case OP_DIV:               \
        fprintf(stream, " / ");  \
        break;

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
    fprintf(fp, "\\documentclass[12pt,a4paper,fleqn]{article}\n"
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
    fprintf(fp, "\n"
                "\\end{document}");
}

static void PrintElemInLatex(Node* node, void* dfs_fp)
{
    FILE* stream = (FILE*)dfs_fp;
    Node_t val = node->val;

    switch (node->val.type)
    {
    case TYPE_VAR:  
        fprintf(stream, "%s", node->val.val.var);
        break;
    case TYPE_OP:
        switch(node->val.val.op)
        {
            PUT_PLUS
            case OP_MUL:
                fprintf(stream, " \\cdot ");
                break;
            PUT_SUB
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
        fprintf(stream, "%lg", node->val.val.dbl);        
        break;
    case UNDEF_NODE_TYPE:
        fprintf(stream, "\n");
        break;
    default:
        fprintf(stream, "\t");
        break;
    }
}

int SaveTreeInLatex(Tree* tree, const char file_name[])
{
    ReturnIfError(TreeCheck(tree));

    FILE* fp = fopen(file_name, "w");
    CHECK(fp == nullptr, "Error during open file", -1);
    
    PrintInLatexStartDoc(fp);
    fprintf(fp, "$$");

    DFS_f pre_function = [](Node* node, void* dfs_fp)
                       {
                            FILE* stream = (FILE*)dfs_fp;
                            Node_t val = node->val;
                            
                            if (val.type == TYPE_OP && VAL_OP(val) == OP_DIV)
                                fprintf(stream, "\\frac{");
                            else if (val.type != TYPE_NUM && val.type != TYPE_VAR)
                                fprintf(stream, "(");
                       };

    DFS_f post_function = [](Node* node, void* dfs_fp)
                        {
                            FILE* stream = (FILE*)dfs_fp;
                            Node_t val = node->val;

                            if (val.type == TYPE_OP && VAL_OP(val) == OP_DIV)
                                fprintf(stream, "}");
                            else if (val.type != TYPE_NUM && val.type != TYPE_VAR)
                                fprintf(stream, ")");
                        };

    DFS(tree->root, pre_function,     fp,
                    PrintElemInLatex, fp,
                    post_function,    fp);

    fprintf(fp, "$$");
    PrintfInLatexEndDoc(fp);

    fclose(fp);

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
    GraphicDump(tree);

    char graphic_dump_file[70] = "";
    #ifdef _WIN32
        sprintf(graphic_dump_file, ".\\GraphicDumps\\Dump%d.png", GRAPHIC_DUMP_CNT - 1);
    #else
        sprintf(graphic_dump_file, "xdg-open GraphicDumps/Dump%d.png", GRAPHIC_DUMP_CNT - 1);
    #endif

    system(graphic_dump_file);
}