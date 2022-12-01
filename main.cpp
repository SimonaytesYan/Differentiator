#include "time.h"

#include "Differentiator/Differentiator.h"
#include "Differentiator/Libs/LatexOutput/LatexOutput.h"
#include "Differentiator/Libs/ConstructGraphics/ConstructGraphics.h"
#include "Differentiator/Libs/Simplifying/Simplifying.h"
#include "Differentiator/Libs/TaylorDecomposition/TaylorDecomposition.h"

void CreateNewList(const char* header)
{
    PrintfInLatex("\\newpage \\textbf{\\LARGE{%s}}\n\n", header);    
}

void CreateChapter1(Tree* tree)
{
    CreateNewList("Глава I. Функция");

    TexNodeWithDesignations(tree->root, "\\begin{center}\n""$y = $");
    PrintfInLatex("\\end{center}\n");

    GrahicParam graphic = {};
    GrahicParamCtor(&graphic, tree, -10, 10, "green");
    ConstructGraphInTex(&graphic);
}

void CreateChapter2(Tree* tree)
{
    CreateNewList("Глава II. Визуальный анализ функции");

    PrintRandBundleInLatex();
    TexNodeWithDesignations(tree->root, "\\begin{center}\n""$y = $");
    PrintfInLatex("\\end{center}\n");
}

void CreateChapter3(Tree* tree, Tree* DTree)
{
    CreateNewList("Глава III. Дифференцирование");

    DTree->root = Diff(tree->root);   
}

void CreateChapter4(Tree* DTree)
{
    CreateNewList("Глава IV.Упрощение выражения");

    SimplifyNode(DTree->root);   
}

void CreateChapter5(Tree* tree, Tree* DTree)
{
    CreateNewList("Глава V. Полученая производная");

    TexNodeWithDesignations(tree->root,  "$y = $");
    TexNodeWithDesignations(DTree->root, "$y' = $");
    
    GrahicParam graphics[2] = {};
    GrahicParamCtor(&graphics[0], tree, -10, 10, "green");
    GrahicParamCtor(&graphics[1], DTree, -10, 10, "red");
    ConstructGraphsInTex(2, graphics);
}

void CreateChapter6(Tree* tree, Tree* Taylor)
{
    CreateNewList("Глава VI. Разложение функции в ряд Тейлора");
    PrintfInLatex("Глава в процессе разработки\n");

    //PrintfInLatex("Глава в процессе разработки\n");
    Node* new_node = GetOneTaylorDecMember(tree->root, 1, 1, "x", 0); 
    //GetFunctionValueAtPoint(tree->root, "x", 2);
    TexNode(new_node);
    /*
    int    TAYLOR_ORDER = 1;
    double TAYLOR_X0    = 0;
    
    FILE* fp = fopen(DEFAULT_TREE_NAME, "r");
    fscanf(fp, "%s");
    fscanf(fp, "%d", &TAYLOR_ORDER);
    fscanf(fp, "%lf", &TAYLOR_X0);
    printf("Start taylor\n");
    Node* deriv = Diff(tree->root);

    Tree TTree = {};
    TreeCtor(&TTree);
    TTree.root = deriv;
    //GraphicDump(&TTree);
    
    Taylor->root = GetOneTaylorDecMember(deriv, 1, 1, "x", 0); 
    //Taylor->root = TaylorDecomp(tree->root, "x", TAYLOR_X0, TAYLOR_ORDER);
    printf("End taylot\n");

    GraphicDump(Taylor);

    PrintfInLatex("\\begin{center}\n");
    PrintfInLatex("$y = $");
    TexNode(Taylor->root);
    PrintfInLatex("$ + o(%d)$\n", TAYLOR_X0)
    PrintfInLatex("\\end{center}")*/
}

int main()
{
    srand(time(NULL));
    printf("Start main\n");
    OpenHtmlLogFile("Diff.log");
    OpenLatexFile(DEFAULT_TEX_NAME);

    Tree tree = {};
    TreeCtor(&tree);
    GetTreeFromFile(&tree, DEFAULT_TREE_NAME);

    CreateChapter1(&tree);
    CreateChapter2(&tree);

    Tree DTree = {};
    TreeCtor(&DTree);
    CreateChapter3(&tree, &DTree);

    CreateChapter4(&DTree);

    CreateChapter5(&tree, &DTree);

    Tree Taylor = {};
    TreeCtor(&Taylor);
    
    CreateChapter6(&tree, &Taylor);

    TreeDtor(&tree);
    TreeDtor(&DTree);
    TreeDtor(&Taylor);
    
    CloseLatexFile();
    CloseHtmlLogFile();
    printf("End main\n");
}