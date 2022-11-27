#include <assert.h>

#include "RecursiveDescent.h"

//----------------------------
//G ::= E ';'
//E ::= T{['+','-']T}*
//T ::= P{['*','/']P}*
//P ::= '('E')' | V
//V ::= ['a'-'z'] | N
//N ::= ['0'-'9']+
//----------------------------

//----------------------------
//+: 5; 2934; 14+99; 5*x; x; 2 + x*(3 + 4542/2) - y 
//-: -5; +7; -19*7; x + u15; 17l; x + y - ; kl; A
//----------------------------

const char* s = nullptr;

Node* GetE();

static Node* GetT();

static Node* GetP();

static Node* GetV();

static Node* GetN();

Node* CreateNodeWithChild_Op(Node* left_node, Node* right_node, OPER_TYPES op);

Node* CreateNodeWithChild_Op(Node* left_node, Node* right_node, OPER_TYPES op)
{
    Node* new_node = NodeCtorOp(op);
    L(new_node)    = left_node;
    R(new_node)    = right_node;

    return new_node;
}

Node* GetG(const char* str)
{
    s = str;

    Node* val = GetE();
    assert(*s == ';');

    return val;
}

Node* GetE()
{
    Node* val = GetT();

    while (*s == '+' || *s == '-')
    {
        char op = *s;
        s++;

        Node* right_node = GetT();

        if (op == '+')
            val = CreateNodeWithChild_Op(val, right_node, OP_PLUS);
        else
            val = CreateNodeWithChild_Op(val, right_node, OP_SUB);
    }

    return val;
}

Node* GetT()
{
    Node* val = GetP();
    while (*s == '*' || *s == '/')
    {
        char op = *s;
        s++;

        Node* right_node = GetP();

        if (op == '*')
            val = CreateNodeWithChild_Op(val, right_node, OP_MUL);
        else
            val = CreateNodeWithChild_Op(val, right_node, OP_DIV);
    }

    return val;
}

Node* GetP()
{
    Node* val = 0;
    if (*s == '(')
    {
        s++;
        val = GetE();
        assert(*s == ')');
        s++;
    }
    else
        val = GetV();

    return val;
}

Node* GetV()
{
    if ('a' <= *s && *s <= 'z')
    {
        char* val = (char*)calloc(2, sizeof(char));
        val[0] = *s;
        s++;
        printf("add var <%s>\n", val);

        return NodeCtorVar(val);
    }
    else
        return GetN();
}

Node* GetN()
{
    int val = 0;
    const char* old_s = s;

    while('0' <= *s && *s <= '9')
    {
        val = val*10 + *s - '0';
        s++;
    }

    assert(s != old_s);
    Node* new_node = NodeCtorNum(val);

    return new_node;
}