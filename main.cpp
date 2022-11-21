#include "time.h"

#include "Differentiator/Differentiator.h"

int main()
{
    srand(time(NULL));
    printf("Start main\n");

    OpenHtmlLogFile("Diff.log");
    OpenLatexFile(DEFAULT_TEX_NAME);

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

    TreeDtor(&tree);
    TreeDtor(&DTree);

    CloseLatexFile();
    printf("Close latex\n");
    
    CloseHtmlLogFile();
    printf("End main\n");
}