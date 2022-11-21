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
    
    SimplifyTree(&tree);

    OutputGraphicDump(&tree);

    printf("Close latex\n");
    
    CloseHtmlLogFile();
    printf("End main\n");
}