#include "time.h"

#include "Differentiator/Differentiator.h"

int main()
{
    srand(time(NULL));
    printf("Star main\n");

    OpenHtmlLogFile("Diff.log");
    OpenLatexFile(DEFAULT_TEX_NAME);

    Tree tree = {};
    TreeCtor(&tree);

    GetTreeFromFile(&tree, DEFAULT_TREE_NAME);
    printf("Tree got\n");

    GraphicDump(&tree);
    PrintfInLatex("\\textbf{\\LARGE Глава I. Функция}\n\n");
    TexNodeWithDesignations(tree.root, "\\begin{center}\n""$y = $");
    PrintfInLatex("\\end{center}\n");

    ConstructGraphInTex(&tree, "green", -10, 10);

    PrintfInLatex("\\newpage \\textbf{\\LARGE Глава II. Зрительный анализ функции}\n\n");
    PrintRandBundleInLatex();
    TexNodeWithDesignations(tree.root, "\\begin{center}\n""$y = $");
    PrintfInLatex("\\end{center}\n");

    PrintfInLatex("\\newpage \\textbf{\\LAGRE Глава III. Дифференцирование}\n\n");
    
    Tree DTree = {};
    TreeCtor(&DTree);

    DeleteNode(DTree.root);
    DTree.root = Diff(tree.root);
    
    PrintfInLatex("\\newpage \\textbf{\\LARGE Глава IV.Упрощение выражения}\n\n");

    SimplifyTree(&DTree);
    
    PrintfInLatex("\\newpage \\textbf{\\LARGE Глава V. Результат вычислений}\n\n");

    
    TexNodeWithDesignations(tree.root, "$y = $");
    TexNodeWithDesignations(DTree.root, "$y' = $");
    

    printf("Close latex\n");

    TreeDtor(&tree);
    TreeDtor(&DTree);
    
    CloseLatexFile();
    CloseHtmlLogFile();
    printf("End main\n");
}