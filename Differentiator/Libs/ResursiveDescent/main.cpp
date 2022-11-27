#include <stdio.h>

#include "RecursiveDescent.h"

#include "../TreeSetup.h"
#include "../BinaryTree/BinaryTree.h"

const int TESTS_NUMBER = 15;

int main()
{
    FILE* fp = fopen("RecursiveDescentTests", "r");
    assert(fp);

    for(int i = 0; i < TESTS_NUMBER; i++)
    {
        char buffer[100] = "";
        fscanf(fp, "%s", buffer);
        Tree tree = {};
        TreeCtor(&tree);
        tree.root = GetG(buffer);

        GraphicDump(&tree);

        TreeDtor(&tree);
    }

}