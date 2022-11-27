#ifndef __SYM_SIMPLFYING_TREE__
#define __SYM_SIMPLFYING_TREE__

#include "../TreeSetup.h"

const int   MAX_DIS_NUM                 = 52;
const int   THRESHOLD_ENTER_DESIGNATION = 10;

void  TexNodeWithDesignations(Node* root, const char pre_decoration[]);

void  SimplifyTree(Tree* tree);

void  ConstsConvolution(Node* node);

void  TexEqualityWithDesignations(Node* node_a, Node* node_b, const char pre_decoration[],
                                                             const char in_decoration[]);

#endif //__SYM_SIMPLFYING_TREE__