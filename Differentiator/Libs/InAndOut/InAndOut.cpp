#include <assert.h>

#include "InAndOut.h"
#include "../ResursiveDescent/RecursiveDescent.h"
#include "../LatexOutput/LatexOutput.h"

void GetDataFromFile(const char* file_name, FileStruct* data_from_file)
{
    assert(data_from_file);
    assert(file_name);

    FILE* fp = fopen(file_name, "r");

    if (fscanf(fp, "%s", data_from_file->tree_str) == EOF)
        return;

    if (fscanf(fp, "%lf %lf", &(data_from_file->x_l), &data_from_file->x_r) == EOF)
        return;
    if (fscanf(fp, "%d", &(data_from_file->taylor_order)) == EOF)
        return;
    fscanf(fp, "%lf", &(data_from_file->taylor_x0));

    fclose(fp);
}

int GetTreeFromFile(Tree* tree, const char file_name[])
{
    assert(tree);

    ReturnIfError(TreeCheck(tree));

    FILE* fp = fopen(file_name, "r");
    CHECK(fp == nullptr, "Error during open file", -1);

    char function[101] = "";
    fgets(function, 100, fp);
    fclose(fp);

    printf("function = <%s>\n", function);

    tree->root = GetNodeFromStr(function);

    return 0;
}

int SaveTreeInFile(Tree* tree, FILE* fp)
{
    ReturnIfError(TreeCheck(tree));
    CHECK(fp == nullptr, "fp = nullptr", -1);

    PrintElemDFS(fp, tree->root);

    fflush(fp);

    return 0;
}

void PrintElemInLog(Node_t elem)
{
    LogPrintf("type = %d\n" "{\n", elem.type);
    LogPrintf("\tdbl  = %lg\n", elem.val.dbl);
    LogPrintf("\top   = %d\n", elem.val.op);
    LogPrintf("\tvar  = <%s?\n", elem.val.var);
    LogPrintf("}\n");
}

void OutputGraphicDump(Tree* tree)
{
    assert(tree);

    GraphicDump(tree);

    char graphic_dump_file[70] = "";
    #ifdef _WIN32
        sprintf(graphic_dump_file, ".\\GraphicDumps\\Dump%d.png", GRAPHIC_DUMP_CNT - 1);
    #else
        sprintf(graphic_dump_file, "xdg-open GraphicDumps/Dump%d.png", GRAPHIC_DUMP_CNT - 1);
    #endif

    system(graphic_dump_file);
}