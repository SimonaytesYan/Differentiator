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

    Tree DTree = {};
    TreeCtor(&DTree);

    DTree.root = Diff(tree.root);
    printf("End diff\n");

    OutputGraphicDump(&DTree);
    printf("End out\n");

    OutputGraphicDump(&tree);

    OpenLatexFile(DEFAULT_TEX_NAME);

    SaveTreeInLatex(&tree);
    SaveTreeInLatex(&DTree);

    TreeDtor(&tree);
    TreeDtor(&DTree);

    CloseLatexFile();
    
    CloseHtmlLogFile();
    printf("End main\n");
}