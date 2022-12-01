#include <stdio.h>
#include <stdarg.h>

#include "LatexOutput.h"
#include "../Bundles.h"

FILE* LatexFp = nullptr;

static void  PrintfInLatexEndDoc();

static void  PrintInLatexStartDoc();

void PrintfInLatexReal(const char* function, const char *format, ...)
{
    if (LatexFp == nullptr)
    {
        fprintf(stderr, "%s\n", function);
        assert(LatexFp == nullptr);
    }

    va_list args;
    va_start(args, format);

    vfprintf(LatexFp, format, args);

    va_end(args);
    fflush(LatexFp);
}

void PrintRandBundleInLatex()
{
    PrintfInLatex("%s", BUNDLES[rand() % BUNDLES_NUMBER]);
}


static void PrintInLatexStartDoc()
{
    PrintfInLatex("\\documentclass[12pt,a4paper,fleqn]{article}\n"
                  "\\usepackage[utf8]{inputenc}\n"
                  "\\usepackage[russian]{babel}\n"
                  "\\usepackage{amssymb, amsmath, multicol}\n"
                  "\\usepackage{enumitem}\n"
                  "\\usepackage{lipsum}\n"
                  "\\usepackage{euler}\n"
                  "\\oddsidemargin=-15.4mm\n"
                  "\\textwidth=190mm\n"
                  "\\headheight=-32.4mm\n"
                  "\\textheight=277mm\n"
                  "\\parindent=0pt\n"
                  "\\parskip=8pt\n"
                  "\\pagestyle{empty}\n"
                  "\\usepackage{graphicx}\n"
                  "\\title{\\textbf{\\LARGE{Исследовательская работа по теме:\\\\"
                  "Исследование функции дифференциальными методами}}}\n"
                  "\\author{Известный гражданин}\n"
                  "\\date{November 2022}\n"
                  "\\addt\\captionsrussian{\\def\\refname{Список литературы}}"

                  "\\begin{document}\n"
                  "\\maketitle\n"
                  "\\newpage"
                 );
}

static void PrintfInLatexEndDoc()
{
    PrintfInLatex("\n"
                  "\\newpage"
                  "\\begin{thebibliography}{}\n"
                  "\\bibitem{link1}  \"A Synopsis of Elementary Results in Pure and Applied Mathematics\"\n"
                  "\\bibitem{link2}  \"Сборник пословиц и поговорок кафедры высшей математики\"\n"
                  "\\bibitem{link3}  \"Полное собрание лучших высказываний преподавателей МФТИ\"\n"
                  "\\bibitem{link4}  \"Словарь фраз не несущих смысловой нагрузки кафедры философии. 17 издание\"\n"
                  "\\end{thebibliography}"
                  "\\end{document}");
}

int OpenLatexFile(const char file_name[])
{
    LatexFp = fopen(file_name, "w");
    CHECK(LatexFp == nullptr, "Error during open file", -1);
    
    PrintInLatexStartDoc();

    return 0;
}

void CloseLatexFile()
{
    PrintfInLatexEndDoc();

    fclose(LatexFp);
    LatexFp = nullptr;
}

void PreFuncTexNode(Node* node, void* useless)
{
    if (IS_OP(node) && VAL_OP(node) == OP_DIV)
        PrintfInLatex("\\frac{")
    else if (!IS_NUM(node) && !IS_VAR(node))
    {
        if (VAL_OP(node) != OP_SIN && VAL_OP(node) != OP_COS && 
            VAL_OP(node) != OP_LOG && VAL_OP(node) != OP_POW)
            PrintfInLatex("(");
    }
}

void PrintElemInLatex(Node* node, void* useless)
{
    assert(node);

    switch (TYPE(node))
    {
    case TYPE_VAR:
        PrintfInLatex("%s", VAL_VAR(node));
        break;
    case TYPE_OP:
        switch(VAL_OP(node))
        {
            case OP_PLUS:
                PrintfInLatex("+");
                break;
            case OP_SUB:
                PrintfInLatex("-");
                break;

            case OP_SIN:
                PrintfInLatex("sin(");
                break;
            
            case OP_COS:
                PrintfInLatex("cos(");
                break;

            case OP_LOG:
                PrintfInLatex("log(");
                break;

            case OP_POW:
                PrintfInLatex("^{");
                break;

            case OP_MUL:
                PrintfInLatex(" \\cdot ");
                break;

            case OP_DIV:
                PrintfInLatex("}{");
                break;
            
            case UNDEF_OPER_TYPE:
                PrintfInLatex("?");
                break;

            default:
                PrintfInLatex("#");
                break;
        }
        break;
    case TYPE_NUM:
        PrintfInLatex("%lg", VAL_N(node));
        break;
    case UNDEF_NODE_TYPE:
        PrintfInLatex("\n");
        break;
    default:
        PrintfInLatex("\t");
        break;
    }
}

void PostFuncTexNode(Node* node, void* useless)
{
    if (IS_OP(node) && (VAL_OP(node) == OP_DIV || VAL_OP(node) == OP_POW))
        PrintfInLatex("}")
    else if (!IS_NUM(node) && !IS_VAR(node))
        PrintfInLatex(")");
}

void TexNode(Node* root)
{
    assert(root);

    #ifdef DEBUG
        printf("Start tex node\n");    
    #endif

    PrintfInLatex("$");

    DFS(root, PreFuncTexNode,   nullptr,
              PrintElemInLatex, nullptr,
              PostFuncTexNode,  nullptr);

    PrintfInLatex("$");
}
