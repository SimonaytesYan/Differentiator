#ifndef __SYM_IN_AND_OUT_DIFF__
#define __SYM_IN_AND_OUT_DIFF__

#include <stdio.h>

#include "../TreeSetup.h"

const int MAX_STR_TREE_SIZE = 100;

struct FileStruct
{
    char   tree_str[MAX_STR_TREE_SIZE] = "";
    double x_l                         = -1;
    double x_r                         = 1;
    int    taylor_order                = 3;
    double taylor_x0                   = 0;
};

void GetDataFromFile(const char* file_name, FileStruct* data_from_file);

int  GetTreeFromFile(Tree* tree, const char file_name[]);

int  SaveTreeInFile(Tree* tree, FILE* fp);

void OutputGraphicDump(Tree* tree);

#endif

