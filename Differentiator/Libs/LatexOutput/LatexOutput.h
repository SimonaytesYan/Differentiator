#ifndef __SYM_LATEX_OUTPUT__
#define __SYM_LATEX_OUTPUT__

#include "../TreeSetup.h"

int  OpenLatexFile(const char file_name[]);

void TexNode(Node* root);

void CloseLatexFile();

void PrintfInLatexReal(const char* function, const char *format, ...);

void PrintRandBundleInLatex();

void PreFuncTexNode(Node* node, void*);

void PrintElemInLatex(Node* node, void*);

void PostFuncTexNode(Node* node, void*);

void PrintElemDFS(FILE* stream, Node* node);

void PrintElem(Node* elem, FILE* stream);

void CreateNewPage(const char* header);

#define PrintfInLatex(format, ...) PrintfInLatexReal(__PRETTY_FUNCTION__, format,##__VA_ARGS__);

#endif