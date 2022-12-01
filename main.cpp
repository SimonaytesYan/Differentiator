#include "time.h"

#include "Differentiator/Differentiator.h"
#include "Differentiator/Libs/LatexOutput/LatexOutput.h"
#include "Differentiator/Libs/ConstructGraphics/ConstructGraphics.h"
#include "Differentiator/Libs/Simplifying/Simplifying.h"
#include "Differentiator/Libs/ResursiveDescent/RecursiveDescent.h"
#include "Differentiator/Libs/TaylorDecomposition/TaylorDecomposition.h"
#include "Differentiator/Libs/InAndOut/InAndOut.h"

void CreateNewPage(const char* header);
void CreateChapter1(Tree* tree, FileStruct* file_data);
void CreateChapter2(Tree* tree);
void CreateChapter3(Tree* tree, Tree* DTree);
void CreateChapter4(Tree* DTree);
void CreateChapter5(Tree* tree, Tree* DTree, FileStruct* file_data);
void CreateChapter6(Tree* tree, Tree* Taylor, FileStruct* file_data);

int main()
{
    srand(time(NULL));
    printf("Start main\n");
    OpenHtmlLogFile("Diff.log");
    OpenLatexFile(DEFAULT_TEX_NAME);

    FileStruct file_data = {};
    GetDataFromFile(DEFAULT_TREE_NAME, &file_data);

    Tree tree = {};
    TreeCtor(&tree);
    tree.root = GetNodeFromStr(file_data.tree_str);

    CreateChapter1(&tree, &file_data);
    CreateChapter2(&tree);

    Tree DTree = {};
    TreeCtor(&DTree);
    CreateChapter3(&tree, &DTree);

    CreateChapter4(&DTree);

    CreateChapter5(&tree, &DTree, &file_data);

    Tree Taylor = {};
    TreeCtor(&Taylor);
    
    CreateChapter6(&tree, &Taylor, &file_data);

    TreeDtor(&tree);
    TreeDtor(&DTree);
    TreeDtor(&Taylor);
    
    CloseLatexFile();
    CloseHtmlLogFile();
    printf("End main\n");
}

void CreateChapter1(Tree* tree, FileStruct* file_data)
{
    CreateNewPage("Глава I. Функция");

    TexNodeWithDesignations(tree->root, "\\begin{center}\n""$y = $");
    PrintfInLatex("\\end{center}\n");

    GrahicParam graphic = {};
    GrahicParamCtor(&graphic, tree, file_data->x_l, file_data->x_r, "green");
    ConstructGraphInTex(&graphic);
}

void CreateChapter2(Tree* tree)
{
    CreateNewPage("Глава II. Визуальный анализ функции");

    PrintRandBundleInLatex();
    TexNodeWithDesignations(tree->root, "\\begin{center}\n""$y = $");
    PrintfInLatex("\\end{center}\n");
}

void CreateChapter3(Tree* tree, Tree* DTree)
{
    CreateNewPage("Глава III. Дифференцирование");

    DTree->root = Diff(tree->root);   
}

void CreateChapter4(Tree* DTree)
{
    CreateNewPage("Глава IV.Упрощение выражения");

    SimplifyNode(DTree->root);   
}

void CreateChapter5(Tree* tree, Tree* DTree, FileStruct* file_data)
{
    CreateNewPage("Глава V. Полученая производная");

    TexNodeWithDesignations(tree->root,  "$y = $");
    TexNodeWithDesignations(DTree->root, "$y' = $");
    
    GrahicParam graphics[2] = {};
    GrahicParamCtor(&graphics[0], tree, file_data->x_l, file_data->x_r, "green");
    GrahicParamCtor(&graphics[1], DTree, file_data->x_l, file_data->x_r, "red");
    ConstructGraphsInTex(2, graphics);
}

void CreateChapter6(Tree* tree, Tree* Taylor, FileStruct* file_data)
{
    CreateNewPage("Глава VI. Разложение функции по формуле Тейлора");
    
    Taylor->root = TaylorDecomp(tree->root, "x", file_data->taylor_x0, file_data->taylor_order);

    PrintfInLatex("\\textbf{\\LARGE{Получим разложение по формуле Тейлора:}}\n")
    PrintfInLatex("\\begin{center}\n");;
    PrintfInLatex("$y = $");
    TexNode(Taylor->root);
    PrintfInLatex("$ + o(x^{%d})$\n", file_data->taylor_order);
    PrintfInLatex("\\end{center}");
}
