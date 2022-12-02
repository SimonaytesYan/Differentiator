#include "ConstructGraphics.h"
#include "../../Differentiator.h"
#include "../LatexOutput/LatexOutput.h"
#include "../InAndOut/InAndOut.h"

static int GraphicCounter();

static int GraphicCounter()
{
    static int counter = 0;
    counter++;

    return counter;
}

void GrahicParamCtor(GrahicParam* graphic_param, Tree*        tree,
                                                 double      left_x_border,
                                                 double      right_x_border,
                                                 const char* color)
{
    graphic_param->tree           = tree;
    graphic_param->left_x_border  = left_x_border;
    graphic_param->right_x_border = right_x_border;
    graphic_param->color          = color;
}

void ConstructGraphInTex(GrahicParam* param)
{
    FILE* fp = fopen(PLOT_PATH, "w");
    
    fprintf(fp, "set terminal jpeg size 600,600\n"
                "set output \"%s.jpg\"\n", PLOT_PATH);
    fprintf(fp, "set grid x\n"
                "set grid y\n"
                "set xrange [%lf:%lf]\n", param->left_x_border, param->right_x_border);
    fprintf(fp, "plot ");

    SaveTreeInFile(param->tree, fp);

    fprintf(fp, " lt rgb \"%s\" lw 2\n", param->color);

    char comand[100] = "";
    sprintf(comand, "gnuplot %s  smooth bezier", PLOT_PATH);

    #ifdef DEBUG
        printf("comand = <%s>\n", comand);
    #endif

    fflush(fp);
    
    system(comand);

    PrintfInLatex("\\includegraphics{%s.jpg}", PLOT_PATH);

    fclose(fp);
}

void ConstructGraphsInTex(int functions_number, GrahicParam* params)
{
    if (functions_number < 0)
        return;
    char file_name[100] = "";
    int  graphic_index = GraphicCounter();

    sprintf(file_name, "%s_%d", PLOT_PATH, graphic_index);
    FILE* fp = fopen(file_name, "w");

    double left_x_border  = params[0].left_x_border;
    double right_x_border = params[0].right_x_border;
    for(int i = 1; i < functions_number; i++)
    {
        if (left_x_border > params[i].left_x_border)
            left_x_border = params[i].left_x_border;
        
        if (right_x_border < params[i].right_x_border)
            right_x_border = params[i].right_x_border;
    }

    fprintf(fp, "set terminal jpeg size 600,600\n"
                "set output \"%s.jpg\"\n", file_name);
    fprintf(fp, "set grid x\n"
                "set grid y\n"
                "set xrange [%lf:%lf]\n", left_x_border, right_x_border);
    fprintf(fp, "plot ");

    for(int i = 0; i < functions_number; i++)
    {
        if (i != 0)
            fprintf(fp, ", ");
        SaveTreeInFile(params[i].tree, fp);

        fprintf(fp, " lt rgb \"%s\" lw 2", params[i].color);
    }
    
    fflush(fp);
    char comand[100] = "";
    sprintf(comand, "gnuplot %s_%d smooth bezier", PLOT_PATH, graphic_index);

    system(comand);
    printf("comand = <%s>\n", comand);
    PrintfInLatex("\\includegraphics{%s_%d.jpg}", PLOT_PATH, graphic_index);

    fclose(fp);
}