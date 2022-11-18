#include <stdio.h>
#include <assert.h>

#include "Differentiator.h"

#define DEBUG

static void GetNodeValFromStr(const char str[], Node_t* val);

static void PrintElem(FILE* stream, Node_t elem);

#define PUT_PLUS                \
    case PLUS_OP:               \
        fprintf(stream, "+");   \
        break;

#define PUT_SUB                \
    case SUB_OP:               \
        fprintf(stream, "-");  \
        break;
        
#define PUT_MUL                 \
    case MUL_OP:                \
        fprintf(stream, "*");   \
        break;
        
#define PUT_DIV                \
    case DIV_OP:               \
        fprintf(stream, "/");  \
        break;

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
        fprintf(stream, "%lf", elem.val.dbl);        
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

static void GetNodeValFromStr(const char str[], Node_t* val)
{
    assert(val);

    if (strcmp(str, "+") == 0)
    {
        #ifdef DEBUG
            printf("plus\n");
        #endif

        val->type   = TYPE_OP;
        val->val.op = PLUS_OP; 
    }
    else if (strcmp(str, "-") == 0)
    {
        #ifdef DEBUG
            printf("minus\n");
        #endif
        val->type   = TYPE_OP;
        val->val.op = SUB_OP;
    }
    else if (strcmp(str, "*") == 0)
    {
        #ifdef DEBUG
            printf("mul\n");
        #endif

        val->type   = TYPE_OP;
        val->val.op = MUL_OP;
    }
    else if (strcmp(str, "/") == 0)
    {
        #ifdef DEBUG
            printf("div\n");
        #endif

        val->type   = TYPE_OP;
        val->val.op = DIV_OP;
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
            printf("num %lf\n", val->val.dbl);
        #endif
    }
}

void GetNodeFromFile(Node** node, FILE* fp)
{
    assert(node);
    assert(fp);

    char c = 0;
    fscanf(fp, "%c", &c);

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
        printf("new node\n");
        Node* new_node = (Node*)calloc(sizeof(Node), 1);

        assert(new_node);

        new_node->left = nullptr;
        new_node->right = nullptr;

        printf("Go to left\n");
        GetNodeFromFile(&(new_node->left), fp);
        printf("End left\n");
        
        while((c = fgetc(fp)) == ')' || c == '(' || c == ' ' || c == '\n' || c == '\t')
        {};
        ungetc(c, fp);

        char new_object[MAX_STR_LEN] = "";
        fscanf(fp, "%[^()\t\n ]", new_object);

        printf("\nnew_object = <%s>\n", new_object);

        GetNodeValFromStr(new_object, &(new_node)->val);

        printf("\nGo to right\n");
        GetNodeFromFile(&(new_node->right), fp);
        printf("End right\n");

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