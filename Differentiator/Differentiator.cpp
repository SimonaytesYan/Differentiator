#include <stdio.h>

#include "Differentiator.h"

#define DEBUG

static void PrintElem(FILE* stream, Node_t elem);

static void PrintElem(FILE* stream, Node_t elem)
{
    switch (elem.type)
    {
    case TYPE_VAR:  
        fprintf(stream, "%s", elem.val.var);
        break;
    case TYPE_OP:
        switch(elem.val.op)
        {
            case PLUS_OP:
                fprintf(stream, "+");
                break;
            case SUB_OP:
                fprintf(stream, "-");
                break;
            case MUL_OP:
                fprintf(stream, "*");
                break;
            case DIV_OP:
                fprintf(stream, "/");
                break;
        }
        break;
    case TYPE_NUM:
        fprintf(stream, "%lf", elem.val.dbl);        
        break;
    }
}

void PrintElemInLog(Node_t elem)
{
    LogPrintf("type = %d\n" "{\n", elem.type);
    LogPrintf("\tdbl  = %lf\n", elem.val.dbl);
    LogPrintf("\top   = %d\n", elem.val.op);
    LogPrintf("\tvar  = <%s?\n", elem.val.var);
    LogPrintf("}\n");
}

int SaveTreeInFile(Tree* tree, const char file_name[])
{
    ReturnIfError(TreeCheck(tree));

    FILE* fp = fopen(file_name, "w");
    CHECK(fp == nullptr, "Error during open file", -1);

    DFS_f pre_function = [](Node* node, void* dfs_fp)
                       {
                            fprintf((FILE*)dfs_fp, "(");
                            PrintElem((FILE*)dfs_fp, node->val);
                       };
    DFS_f post_function = [](Node*, void* dfs_fp)
                        {
                            fprintf((FILE*)dfs_fp, ")");
                        };

    DFS(tree->root, pre_function,  fp,
                    nullptr,       nullptr,
                    post_function, fp);

    fclose(fp);

    return 0;
}

void GetNodeFromFile(Node* node, void* fp_void)
{
    FILE* fp = (FILE*)fp_void;

    char new_str_object[MAX_STR_LEN] = "";

    int c = getc(fp);
    while(c != '(' && c != EOF)
        c = getc(fp);

    if (c == EOF) return;

    while((c = getc(fp)) == ' ') 
    {
        if (c == EOF)
            return;
    };
    ungetc(c, fp);

    int i = 0;
    while ((c = getc(fp)) != '(' && c != EOF && c != ')' && c != ' ')
    {
        new_str_object[i] = (char)c;
        i++;
    }
    new_str_object[i] = '\0';

    if (c == EOF) return;

    printf("<%s>\n", new_str_object);

    if (strcmp(new_str_object, "+") == 0)
    {
        #ifdef DEBUG
            printf("plus\n");
        #endif

        node->val.type   = TYPE_OP;
        node->val.val.op = PLUS_OP; 
    }
    else if (strcmp(new_str_object, "-") == 0)
    {
        #ifdef DEBUG
            printf("minus\n");
        #endif
        node->val.type   = TYPE_OP;
        node->val.val.op = SUB_OP;
    }
    else if (strcmp(new_str_object, "*") == 0)
    {
        #ifdef DEBUG
            printf("mul\n");
        #endif

        node->val.type   = TYPE_OP;
        node->val.val.op = MUL_OP;
    }
    else if (strcmp(new_str_object, "/") == 0)
    {
        #ifdef DEBUG
            printf("div\n");
        #endif

        node->val.type   = TYPE_OP;
        node->val.val.op = DIV_OP;
    }
    else if (atof(new_str_object) == 0 && strcmp(new_str_object, "0"))
    {
        #ifdef DEBUG
            printf("var\n");
        #endif

        node->val.type    = TYPE_VAR;
    
        node->val.val.var = (char*)calloc(1, strlen(new_str_object));
        strcpy(node->val.val.var, new_str_object);
    }
    else
    {
        #ifdef DEBUG
            printf("atof = %lf\n", atof(new_str_object));
        #endif

        node->val.type    = TYPE_NUM;
        node->val.val.dbl = atof(new_str_object);
        
        #ifdef DEBUG
            printf("num %lf\n", node->val.val.dbl);
        #endif
    }

    while (c == ' ' && c != EOF)
        c = getc(fp);

    if (c == EOF) return;

    #ifdef DEBUG
        printf("c = <%c>%d\n", c, c);
    #endif
    
    if (c == ')')
    {
        node->left  = nullptr;
        node->right = nullptr;
    }
    else
    {
        ungetc(c, fp);
        node->left  = (Node*)calloc(sizeof(Node), 1);
        node->right = (Node*)calloc(sizeof(Node), 1);
    }
}

int GetTreeFromFile(Tree* tree, const char file_name[])
{
    ReturnIfError(TreeCheck(tree));

    FILE* fp = fopen(file_name, "r");
    CHECK(fp == nullptr, "Error during open file", -1);

    DFS(tree->root, GetNodeFromFile, fp,
                    nullptr,         nullptr,
                    nullptr,         nullptr);

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