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

    PrintfInLatex("\\textbf{\\LARGE Глава I. Функция}\n\n");
    PrintfInLatex("\\begin{center}\n""$y = $");
    TexNodeWithDesignations(tree.root);
    PrintfInLatex("\\end{center}\n");

    PrintfInLatex("\\newpage \\textbf{\\LARGE Глава II. Зрительный анализ функции}\n\n");
    PrintRandBundleInLatex();
    PrintfInLatex("\\begin{center}\n""$y = $");
    TexNodeWithDesignations(tree.root);
    PrintfInLatex("\\end{center}\n");

    PrintfInLatex("\\newpage \\textbf{\\LAGRE Глава III. Дифференцирование}\n\n");
    
    Tree DTree = {};
    TreeCtor(&DTree);

    DeleteNode(DTree.root);
    DTree.root = Diff(tree.root);
    
    PrintfInLatex("\\newpage \\textbf{\\LARGE Глава IV.Упрощение выражения}\n\n");

    SimplifyTree(&DTree);
    
    PrintfInLatex("\\newpage \\textbf{\\LARGE Глава V. Результат}\n\n");

    PrintfInLatex("$y = $");
    TexNode(DTree.root);
    TexNodeWithDesignations(DTree.root);
    //PrintfInLatex("\\newpage \\textbf{\\LARGE Список литературы}\n\n");

    printf("Close latex\n");

    TreeDtor(&tree);
    TreeDtor(&DTree);
    
    CloseLatexFile();
    CloseHtmlLogFile();
    printf("End main\n");
}