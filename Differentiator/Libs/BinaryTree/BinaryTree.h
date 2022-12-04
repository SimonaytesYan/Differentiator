#ifndef __SYM_BINARY_TREE__
#define __SYM_BINARY_TREE__

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "../Logging/Logging.h"
#include "../Errors.h"
#include "../DSL.h"

static const char COMAND_PROTOTYPE[] = "dot GraphicDumps/dump%d -o GraphicDumps/Dump%d.png -T png";
static       int  GRAPHIC_DUMP_CNT   = 0;
static const int  MAX_VAR_NAME_LEN   = 10; 

typedef struct Node 
{
    Node_t val   = {};

    Node*  left   = nullptr;
    Node*  right  = nullptr;
} Node;

typedef void(*DFS_f)(Node* node, void*);

typedef struct LogInfo 
{
    const char* name     = "(null)";
    const char* function = "(null)";
    const char* file     = "(null)";
    int         line     = -1;
    bool        status   = false;
} LogInfo;

typedef struct Tree 
{
    Node*   root = nullptr;
    LogInfo debug     = {};
}Tree;

static int   NodeCtor(Node* node, Node_t val);

static Node* NodeCtorNum(double val);

static Node* NodeCtorOp(OPER_TYPES val);

static Node* NodeCtorVar(const char* val);

static int   NodeCmp(Node* node_a, Node* node_b);

static Node* CpyNode(Node* node);

static int   TreeCheck(Tree* tree);

static int   TreeConstructor(Tree* tree, int capacity, int line, const char* name, const char* function, const char* file);
 
static int   TreeDtor(Tree* tree);
 
static void  DumpTree(Tree* tree, const char* function, const char* file, int line);
 
static void  GraphicDump(Tree* tree);
 
static int   TreeInsertLeafRight(Tree* tree, const Node_t value, Node* after_which);
 
static int   TreeInsertLeafLeft(Tree* tree, const Node_t value, Node* after_which);
 
static int   TreeIterate(Tree* tree, Node** index);
 
static void  DFS(Node* node, DFS_f pre_func, void* args1, DFS_f in_func, void* args2, DFS_f post_func, void* args3);
 
static void  DeleteNode(Node* node);
 
static void  DFSNodeDtor(Node* node);

static int   CmpNode_t(Node_t* a, Node_t* b);

static int   CmpNode_t(Node_t* a, Node_t* b)
{
    if (a->type != b->type)
        return 1;
    
    switch (a->type)
    {
        case TYPE_NUM:
            return !IS_DOUBLE_EQ(a->val.dbl, b->val.dbl);
        case TYPE_OP:
            return a->val.op  != b->val.op;
        case TYPE_VAR:
            return strcmp(a->val.var, b->val.var);
        
        case UNDEF_NODE_TYPE:
            return 0;
        default:
            return 0;
    }
}

int NodeCmp(Node* node_a, Node* node_b)
{
    if (node_a == nullptr && node_b == nullptr)
        return 0;
    if (node_a == nullptr || node_b == nullptr)
        return 1;

    if (CmpNode_t(&(node_a->val), &(node_b->val)))
        return 1;

    if (NodeCmp(node_a->left, node_b->left) != 0)
        return 1;
    
    if (NodeCmp(node_a->right, node_b->right) != 0)
        return 1;
    
    return 0;
}

static void DFS(Node* node, DFS_f pre_func, void* args1, DFS_f in_func, void* args2, DFS_f post_func, void* args3)
{
    if (node == nullptr)
        return;

    if (pre_func != nullptr)
        pre_func(node, args1);

    DFS(node->left, pre_func, args1, in_func, args2, post_func, args3);

    if (in_func != nullptr)
        in_func(node, args2);
    
    DFS(node->right, pre_func, args1, in_func, args2, post_func, args3);

    if (post_func != nullptr)
        post_func(node, args3);
}

static void WriteNodeAndEdge(Node* node, void* fp_void)
{ 
    assert(node);
    FILE* fp = (FILE*)fp_void;

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wformat"
    fprintf(fp, "Node%06X[style = \"filled,rounded\", fillcolor = \"#B1FF9F\", label = \"{<i>%06X \\n | { <v>",
                     node,                                                                      node);
    switch (node->val.type)
    {
    case TYPE_NUM:
        fprintf(fp, "NUM | %lf", node->val.val.dbl);
        break;
    case TYPE_OP:
        fprintf(fp, "OPER | ");
        switch(node->val.val.op)
        {
            case OP_COS:
                fprintf(fp, "cos");
                break;
            case OP_SIN:
                fprintf(fp, "sin");
                break;
            case OP_PLUS:
                fprintf(fp, "+");
                break;
            case OP_SUB:
                fprintf(fp, "-");
                break;
            case OP_MUL:
                fprintf(fp, "*");
                break;
            case OP_DIV:
                fprintf(fp, "/");
                break;
            case OP_LOG:
                fprintf(fp, "log");
                break;
            case OP_POW:
                fprintf(fp, "**");
                break;
            case UNDEF_OPER_TYPE:
                fprintf(fp, "?");
                break;
            default:
                fprintf(fp, "#");
                break;
        }
        break;
    case TYPE_VAR:
        fprintf(fp, "VAR | %s ", node->val.val.var);
        break;
    case UNDEF_NODE_TYPE:
        fprintf(fp, "UNDEF");
        break;
    default:
        fprintf(fp, "unknown");
        break;
    }

    fprintf(fp, "} | { <l> %06X  |<r>  %06X}}\"]\n",
                 node->left, node->right);
    
    if (node->left != nullptr)
        fprintf(fp, "Node%06X -> Node%06X[xlabel = \"L\"]\n", node, node->left);
    if (node->right != nullptr)
        fprintf(fp, "Node%06X -> Node%06X[xlabel = \"R\"]\n", node, node->right);
    #pragma GCC diagnostic pop
}

static void GraphicDump(Tree* tree)
{
    assert(tree);
    printf("Graphic dump\n");

    char name[30] = "";
    sprintf(name, "GraphicDumps/dump%d", GRAPHIC_DUMP_CNT);
    FILE* fp = fopen(name, "w");

    fprintf(fp, "digraph G{\n");
    fprintf(fp, "node[shape = record, fontsize = 14];\n");
    fprintf(fp, "splines = ortho\n");

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wformat"
    fprintf(fp, "info[label = \"root = 0x%06X\"]\n", tree->root);
    #pragma GCC diagnostic pop

    DFS(tree->root, WriteNodeAndEdge, fp, 
                    nullptr,          nullptr,
                    nullptr,          nullptr);

    fprintf(fp, "}");

    fclose(fp);

    char comand[100] = "";
    sprintf(comand, COMAND_PROTOTYPE, GRAPHIC_DUMP_CNT, GRAPHIC_DUMP_CNT);
    system(comand);
    
    LogPrintf("<hr>");
    LogPrintf("<img src=\"GraphicDumps/Dump%d.png\">\n", GRAPHIC_DUMP_CNT);
    LogPrintf("<hr>");

    GRAPHIC_DUMP_CNT++;
}

#define DUMP_L(tree) DumpTree(tree, __PRETTY_FUNCTION__, __FILE__, __LINE__)

static void DumpTree(Tree* tree, const char* function, const char* file, int line)
{
    LogPrintf("\nDump in %s(%d) in function %s\n", file, line, function);

    TreeCheck(tree);
    
    if (tree == nullptr) 
        return;

    LogPrintf("Stack[%p] ", tree);

    if (tree->debug.name != nullptr)
        LogPrintf("\"%s\"", tree->debug.name);
    if (tree->debug.function != nullptr)
        LogPrintf("created at %s ", tree->debug.function);
    if (tree->debug.file != nullptr)
        LogPrintf("at %s(%d):", tree->debug.file);
    LogPrintf("\n");

    LogPrintf("Status: ");
    if (tree->debug.status)
        LogPrintf("enable\n");
    else
        LogPrintf("disable\n");

    LogPrintf("{\n");

    DFS_f pre_function = [](Node* node, void*)
                       { 
                           LogPrintf("(");
                           PrintElemInLog(node->val);
                       };
    DFS_f post_function = [](Node*, void*)
                        {
                            LogPrintf(")");
                        };

    DFS(tree->root, pre_function,  nullptr,
                    nullptr,       nullptr,
                    post_function, nullptr);

    LogPrintf("\n}\n\n");
}

static int TreeCheck(Tree* tree)
{
    int error = 0;
    if (tree == nullptr)
        error |= NULL_TREE_POINTER;
    else
    {
        if (tree->root == nullptr)
            error |= NULL_POINTER_TO_ROOT;
        
        if (tree->debug.file     == nullptr) error |= DEBUG_FILE_DAMAGED;
        if (tree->debug.function == nullptr) error |= DEBUG_FUNCTION_DAMAGED;
        if (tree->debug.name     == nullptr) error |= DEBUG_NAME_DAMAGED;
    }

    LogAndParseErr(error != 0, error);
    
    return error;
}

#define TreeCtor(tree) TreeConstructor(tree, __LINE__, #tree, __PRETTY_FUNCTION__, __FILE__)

static int TreeConstructor(Tree* tree, int line, const char* name, const char* function, const char* file)
{
    LogAndParseErr(tree == nullptr, NULL_TREE_POINTER);

    tree->root           = nullptr;

    #ifdef DEBUG
        printf("Ctor(tree.root) = %p\n", tree->root);
    #endif

    tree->debug.name     = name;
    tree->debug.function = function;
    tree->debug.file     = file;
    tree->debug.line     = line;
    tree->debug.status   = true;

    return TreeCheck(tree);
}

static int NodeCtor(Node* node, Node_t val)
{
    assert(node);

    node->val   = val;
    node->left  = nullptr;
    node->right = nullptr;

    return 0;
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

static Node* NodeCtorVar(const char* val)
{
    Node* new_node = (Node*)calloc(1, sizeof(Node));

    Node_t node_val  = {};
    node_val.type    = TYPE_VAR;

    node_val.val.var = (char*)calloc(strlen(val) + 1, 1);
    strcpy(node_val.val.var, val);

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

static void NodeDtor(Node* node)
{
    if (node == nullptr)
        return;
    
    if (node->val.type == TYPE_VAR)
        free(node->val.val.var);
    free(node);
}

static void DeleteNode(Node* node)
{
    DFSNodeDtor(node);
    NodeDtor(node);
}

static void DFSNodeDtor(Node* node)
{
    if (node == nullptr)
        return;
    
    DFSNodeDtor(node->left);
    DFSNodeDtor(node->right);

    NodeDtor(node->left);
    NodeDtor(node->right);
    node->left  = nullptr;
    node->right = nullptr;
}

static int TreeDtor(Tree* tree)
{
    TreeCheck(tree);
    
    #ifdef DEBUG
        printf("Dtor(tree.root) = %p\n", tree->root);
    #endif

    DeleteNode(tree->root);
    tree->root = nullptr;

    tree->debug.file     = nullptr;
    tree->debug.function = nullptr;
    tree->debug.name     = nullptr;
    tree->debug.line     = 0;

    return 0;
}

static Node* CpyNode(Node* node)
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


static int TreeInsertLeafRight(Tree* tree, const Node_t value, Node* after_which)
{
    ReturnIfError(TreeCheck(tree));
    CHECK(after_which == nullptr, "after_which = nullptr", -1);

    Node* new_elem = (Node*)calloc(1, sizeof(Node));
    memcpy(&(new_elem->val), &value, sizeof(value));

    after_which->right = new_elem;

    return 0;
}

static int TreeInsertLeafLeft(Tree* tree, const Node_t value, Node* after_which)
{
    ReturnIfError(TreeCheck(tree));
    CHECK(after_which == nullptr, "after_which = nullptr", -1);

    Node* new_elem = (Node*)calloc(1, sizeof(Node));
    memcpy(&(new_elem->val), &value, sizeof(value));

    after_which->left = new_elem;

    return 0;
}

#pragma GCC diagnostic pop

#endif //__SYM_BINARY_TREE__