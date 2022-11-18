#include "Differentiator/Differentiator.h"

int main()
{
    printf("Start main\n");
    OpenHtmlLogFile("Diff.log");

    Tree tree = {};
    TreeCtor(&tree);
    GetTreeFromFile(&tree, DEFAULT_TREE_NAME);
    printf("Tree got\n");

    OutputGraphicDump(&tree);
    
    SaveTreeInFile(&tree, DEFAULT_TREE_NAME);
    SaveTreeInLatex(&tree, "Zorich_3_volume.tex");
    TreeDtor(&tree);
    
    CloseHtmlLogFile();
    printf("End main\n");
}