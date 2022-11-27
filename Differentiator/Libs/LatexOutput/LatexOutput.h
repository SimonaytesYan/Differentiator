#ifndef __SYM_LATEX_OUTPUT__
#define __SYM_LATEX_OUTPUT__

#include "../TreeSetup.h"

int  OpenLatexFile(const char file_name[]);

void TexNode(Node* root);

void CloseLatexFile();

void PrintfInLatexReal(const char* function, const char *format, ...);

void PrintRandBundleInLatex();

void PreFuncTexNode(Node* node, void* useless);

void PrintElemInLatex(Node* node, void* useless);

void PostFuncTexNode(Node* node, void* useless);

#define PrintfInLatex(format, ...) PrintfInLatexReal(__PRETTY_FUNCTION__, format,##__VA_ARGS__);

#endif