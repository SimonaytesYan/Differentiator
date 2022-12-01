#include "../TreeSetup.h"
#include "../BinaryTree/BinaryTree.h"

struct Function_t
{
    const char*      name = "";
    const OPER_TYPES code = UNDEF_OPER_TYPE; 
};

Node* GetNodeFromStr(const char* str);