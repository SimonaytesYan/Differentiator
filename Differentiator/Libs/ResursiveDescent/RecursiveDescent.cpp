#include <assert.h>

#include "RecursiveDescent.h"
#include "../Logging/Logging.h"

#define DEBUG

const char* FIRST_ELEM_PTR = nullptr;

//----------------------------
//G   ::= E ';'
//E   ::= T{['+','-']T}*
//T   ::= P{['*','/']P}*
//O   ::= {"sin" | "cos" | "log"}?POW
//POW ::= P {"**"O}*
//P   ::= '('E')' | V
//V   ::= ['a'-'z'] | N
//N   ::= ['0'-'9']+
//----------------------------

//----------------------------
//+: x**2; x**x**x; y+sin(x**2); 5; 2934; 14+99; 5*x; x; 2 + x*(3 + 4542/2) - y; sin(sin(x)); y + sin(a * cos(log(1))) 
//-: -5; +7; -19*7; x + u15; 17l; x + y - ; kl; A
//----------------------------

const int STD_FUNCTION_NUM = 3;

const Function_t STD_FUNCTION[] = {
                                    {"sin", OP_SIN},
                                    {"cos", OP_COS},
                                    {"log", OP_LOG},
                                   };

#define CheckSyntaxError(cond, s)                                                       \
    if (!(cond))                                                                        \
    {                                                                                   \
        LogPrintf("Syntax error in symbol %d: %s\n", s - FIRST_ELEM_PTR , #cond);       \
        fprintf(stderr, "Syntax error in symbol %d: %s\n", s - FIRST_ELEM_PTR , #cond); \
        return nullptr;                                                                 \
    }

static Node* GetE(const char** s);

static Node* GetT(const char** s);

static Node* GetP(const char** s);

static Node* GetV(const char** s);

static Node* GetN(const char** s);

static Node* GetO(const char** s);

static Node* GetPow(const char** s);

Node* CreateNodeWithChild_Op(Node* left_node, Node* right_node, OPER_TYPES op);

Node* CreateNodeWithChild_Op(Node* left_node, Node* right_node, OPER_TYPES op)
{
    Node* new_node = NodeCtorOp(op);
    L(new_node)    = left_node;
    R(new_node)    = right_node;

    return new_node;
}

Node* GetNodeFromStr(const char* str)
{
    #ifdef DEBUG
        printf("(G) s = <%s>\n", str);
    #endif

    FIRST_ELEM_PTR = str;
    Node* val = GetE(&str);
    if (val == nullptr) return nullptr;

    printf("val = %p\n", val);
    CheckSyntaxError(*str == ';', str);

    return val;
}

Node* GetE(const char** s)
{
    #ifdef DEBUG
        printf("(E) s = <%s>\n", *s);
    #endif

    Node* val = GetT(s);
    if (val == nullptr) return nullptr;

    while (**s == '+' || **s == '-')
    {
        char op = **s;
        (*s)++;

        Node* right_node = GetT(s);
        if (right_node == nullptr) return nullptr;

        if (op == '+')
            val = CreateNodeWithChild_Op(val, right_node, OP_PLUS);
        else
            val = CreateNodeWithChild_Op(val, right_node, OP_SUB);
    }

    #ifdef DEBUG
        printf("end E\n");
    #endif
    return val;
}

Node* GetT(const char** s)
{
    #ifdef DEBUG
        printf("(T) s = <%s>\n", *s);
    #endif

    Node* val = GetO(s);
    if (val == nullptr) return nullptr;

    while (**s == '*' || **s == '/')
    {
        char op = **s;
        (*s)++;

        Node* right_node = GetO(s);
        if (right_node == nullptr) return nullptr;

        if (op == '*')
            val = CreateNodeWithChild_Op(val, right_node, OP_MUL);
        else
            val = CreateNodeWithChild_Op(val, right_node, OP_DIV);
    }

    #ifdef DEBUG
        printf("end T\n");
    #endif

    return val;
}

Node* GetO(const char** s)
{
    #ifdef DEBUG
        printf("(O) s = <%s>\n", *s);
    #endif

    const char* old_s = *s;

    //!ToDO сделать табличку функций в языке и номеров функций в коде 

    Node* node = nullptr;
    bool function_found = false;
    for(int i = 0; i < STD_FUNCTION_NUM; i++)
    {
        size_t lenght = strlen(STD_FUNCTION[i].name);

        if (!strncmp(*s, STD_FUNCTION[i].name, lenght))
        {
            node = NodeCtorOp(STD_FUNCTION[i].code);
            (*s) += lenght;
            R(node) = GetPow(s);
            if (R(node) == nullptr) return nullptr;

            function_found = true;
            break;
        }
    }

    if (!function_found)
    {
        node = GetPow(s);
        if (node == nullptr) return nullptr;
    }

    CheckSyntaxError(*s != old_s, *s);

    #ifdef DEBUG
        printf("end O\n");
    #endif
    
    return node;
}

Node* GetPow(const char** s)
{
    #ifdef DEBUG
        printf("(POW) s = <%s>\n", *s);
    #endif
    Node* node = GetP(s);
    if (node == nullptr) return nullptr;

    while (!strncmp(*s, "**", 2))
    {
        (*s)+=2;
        Node* node_right = GetO(s);
        if (node_right == nullptr) return nullptr;

        node = CreateNodeWithChild_Op(node, node_right, OP_POW);
    }
    
    return node;
}

Node* GetP(const char** s)
{
    #ifdef DEBUG
        printf("(P) s = <%s>\n", *s);
    #endif

    Node* val = 0;
    if (**s == '(')
    {
        (*s)++;
        val = GetE(s);
        if (val == nullptr) return nullptr;

        CheckSyntaxError(**s == ')', *s);
        (*s)++;
    }
    else
    {
        val = GetV(s);
        if (val == nullptr) return nullptr;
    }

    #ifdef DEBUG
        printf("end P\n");
    #endif

    return val;
}

Node* GetV(const char** s)
{
    #ifdef DEBUG
        printf("(V) s = <%s>\n", *s);
    #endif

    Node* node = nullptr;
    if ('a' <= **s && **s <= 'z')
    {
        char* val = (char*)calloc(2, sizeof(char));
        val[0] = **s;
        (*s)++;

        node = NodeCtorVar(val);
    }
    else
    {
        node = GetN(s);
        if (node == nullptr) return nullptr;
    }
    
    #ifdef DEBUG
        printf("end V\n");
    #endif

    return node;
}

Node* GetN(const char** s)
{
    #ifdef DEBUG
        printf("(N) s = <%s>\n", *s);
    #endif

    int val = 0;
    const char* old_s = *s;

    while('0' <= **s && **s <= '9')
    {
        val = val*10 + **s - '0';
        (*s)++;
    }

    CheckSyntaxError(*s != old_s, *s);
    Node* new_node = NodeCtorNum(val);

    #ifdef DEBUG
        printf("end N\n");
    #endif

    return new_node;
}