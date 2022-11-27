#ifndef __DIFFERENTIATOR_SYM__
#define __DIFFERENTIATOR_SYM__

#include "Libs/Logging/Logging.h"

#include "Libs/TreeSetup.h"

const int   MAX_STR_LEN                 = 20;
const char  DEFAULT_TREE_NAME[]         = "Tree";
const char  DEFAULT_TEX_NAME[]          = "Zorich_3_volume.tex";

int   SaveTreeInFile(Tree* tree, FILE* fp);

int   GetTreeFromFile(Tree* tree, const char file_name[]);

void  OutputGraphicDump(Tree* tree);

Node* Diff(Node* node);

#endif //__DIFFERENTIATOR_SYM__