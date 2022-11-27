#include "time.h"

#include "Differentiator/Differentiator.h"
#include "Differentiator/Libs/LatexOutput/LatexOutput.h"
#include "Differentiator/Libs/ConstructGraphics/ConstructGraphics.h"
#include "Differentiator/Libs/Simplifying/Simplifying.h"

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

    GrahicParam graphic = {};
    GrahicParamCtor(&graphic, &tree, -10, 10, "green");
    ConstructGraphInTex(&graphic);

    PrintfInLatex("\\newpage \\textbf{\\LARGE Глава II. Визуальный анализ функции}\n\n");
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
    
    PrintfInLatex("\\newpage \\textbf{\\LARGE Глава V. Полученая производная}\n\n");

    TexNodeWithDesignations(tree.root,  "$y = $");
    TexNodeWithDesignations(DTree.root, "$y' = $");
    
    GrahicParam graphics[2] = {};
    GrahicParamCtor(&graphics[0], &tree, -10, 10, "green");
    GrahicParamCtor(&graphics[1], &DTree, -10, 10, "red");
    ConstructGraphsInTex(2, graphics);

    //ConstructGraphInTex(&(graphics[1]));

    printf("Close latex\n");

    TreeDtor(&tree);
    TreeDtor(&DTree);
    
    CloseLatexFile();
    CloseHtmlLogFile();
    printf("End main\n");
}