#ifndef __SYM_GRAPH_CONSTRUCT__
#define __SYM_GRAPH_CONSTRUCT__

#include "../TreeSetup.h"

const char  PLOT_PATH[] = "GraphicDumps/plot";

struct GrahicParam
{
    Tree*       tree           = nullptr;
    double      left_x_border  = -10;
    double      right_x_border = 10;
    const char* color          = "red";
};

void GrahicParamCtor(GrahicParam* graphic_param, Tree*        tree,
                                                 double      left_x_border,
                                                 double      right_x_border,
                                                 const char* color);

void ConstructGraphInTex(GrahicParam* param);

void ConstructGraphsInTex(int functions_number, GrahicParam* params);

#endif