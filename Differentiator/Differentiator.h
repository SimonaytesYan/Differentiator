#ifndef __DIFFERENTIATOR_SYM__
#define __DIFFERENTIATOR_SYM__

#include "Libs/Logging/Logging.h"

#include "Libs/TreeSetup.h"

const int   MAX_STR_LEN                 = 20;
const int   MAX_DIS_NUM                 = 52;
const int   THRESHOLD_ENTER_DESIGNATION = 10;
const char  DEFAULT_TREE_NAME[]         = "Tree";
const char  DEFAULT_TEX_NAME[]          = "Zorich_3_volume.tex";
const char  PLOT_PATH[]                 = "GraphicDumps/plot";


int   SaveTreeInFile(Tree* tree, FILE* fp);

void  GetNodeFromFile(Node** new_node, FILE* fp);

int   GetTreeFromFile(Tree* tree, const char file_name[]);

void  OutputGraphicDump(Tree* tree);

int   OpenLatexFile(const char file_name[]);

void  TexNode(Node* root);

void  TexNodeWithDesignations(Node* root, const char pre_decoration[]);

void  CloseLatexFile();

Node* Diff(Node* node);

Node* CpyNode(Node* node);

void  PrintfInLatex(const char *format, ...);

void  SimplifyTree(Tree* tree);

void  ConstsConvolution(Node* node);

void PrintfInLatexReal(const char* function, const char *format, ...);

void PrintRandBundleInLatex();

void ConstructGraphInTex(Tree* tree, const char color[], int left_x_border, int right_x_border);

#define PrintfInLatex(format, ...) PrintfInLatexReal(__PRETTY_FUNCTION__, format,##__VA_ARGS__);

#endif //__DIFFERENTIATOR_SYM__