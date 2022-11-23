#include "time.h"

#include "Differentiator/Differentiator.h"

int main()
{
    srand(time(NULL));
    printf("Starting main\n");

    OpenHtmlLogFile("Diff.log");
    OpenLatexFile(DEFAULT_TEX_NAME);

    Tree tree = {};
    TreeCtor(&tree);

    printf("start getting tree\n");
    GetTreeFromFile(&tree, DEFAULT_TREE_NAME);
    printf("Tree got\n");

    GraphicDump(&tree);
    printf("Tree dumped\n");

    
    PrintfInLatex("$y = $");

    TexNode(tree.root);
    PrintfInLatex("\n\n Продиффиринцируем эту функцию\n\n");

    Tree DTree = {};
    TreeCtor(&DTree);
    
    DTree.root = Diff(tree.root);
    
    SimplifyTree(&DTree);
    
    PrintfInLatex("\n\n Получаем выражение\n\n");

    TexNode(DTree.root);

    printf("Close latex\n");

    TreeDtor(&tree);
    TreeDtor(&DTree);
    
    CloseLatexFile();
    CloseHtmlLogFile();
    printf("End main\n");
}