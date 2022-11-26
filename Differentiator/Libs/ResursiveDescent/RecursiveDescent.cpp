#include <assert.h>

#include "RecursiveDescent.h"

const char* s = nullptr;

int GetG(const char* str)
{
    s = str;

    int val = GetE();
    assert(*s == '\0');

    return val;
}

int GetE()
{
    int val = GetT();

    while (*s == '+' || *s == '-')
    {
        char op = *s;
        s++;

        int val2 = GetT();

        if (op == '+')
            val += val2;
        else
            val -= val2;
    }

    return val;
}

int GetT()
{
    int val = GetP();

    while (*s == '*' || *s == '/')
    {
        char op = *s;
        s++;

        int val2 = GetP();

        if (op == '*')
            val *= val2;
        else
            val /= val2;
    }

    return val;
}

int GetP()
{
    int val = 0;
    if (*s == '(')
    {
        s++;
        val = GetE();
        assert(*s == ')');
        s++;
    }
    else
    {
        val = GetN();
    }

    return val;
}

int GetN()
{
    int val = 0;
    const char* old_s = s;

    while('0' <= *s && *s <= '9')
    {
        val = val*10 + *s - '0';
        s++;
    }

    assert(s != old_s);

    return val;
}