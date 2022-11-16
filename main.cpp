#include "Differentiator/Differentiator.h"

int main()
{
    OpenHtmlLogFile("Diff.log");

    Tree tree = {};
    TreeCtor(&tree);
    GetTreeFromFile(&tree, DEFAULT_TREE_NAME);

    OutputGraphicDump(&tree);
    
    SaveTreeInFile(&tree, DEFAULT_TREE_NAME);
    TreeDtor(&tree);
    
    CloseHtmlLogFile();
}