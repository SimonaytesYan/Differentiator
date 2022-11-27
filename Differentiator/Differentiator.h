#ifndef __DIFFERENTIATOR_SYM__
#define __DIFFERENTIATOR_SYM__

#include "Libs/Logging/Logging.h"

#include "Libs/TreeSetup.h"

const int   MAX_STR_LEN                 = 20;
const int   MAX_DIS_NUM                 = 52;
const int   THRESHOLD_ENTER_DESIGNATION = 10;
const char  DEFAULT_TREE_NAME[]         = "Tree";
const char  DEFAULT_TEX_NAME[]          = "Zorich_3_volume.tex";

int   SaveTreeInFile(Tree* tree, FILE* fp);

int   GetTreeFromFile(Tree* tree, const char file_name[]);

void  OutputGraphicDump(Tree* tree);

void  TexNodeWithDesignations(Node* root, const char pre_decoration[]);

Node* Diff(Node* node);

void  PrintfInLatex(const char *format, ...);

void  SimplifyTree(Tree* tree);

void  ConstsConvolution(Node* node);


#endif //__DIFFERENTIATOR_SYM__