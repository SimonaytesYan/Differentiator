#include <stdio.h>
#include <stdarg.h>

#include "LatexOutput.h"
#include "../Bundles.h"

FILE* LatexFp = nullptr;

static void  PrintfInLatexEndDoc();

static void  PrintInLatexStartDoc();

static int   GetOpRank(OPER_TYPES operation);

static bool PrintBracketL(Node* node);

static bool PrintBracketR(Node* node);

void CreateNewPage(const char* header)
{
    PrintfInLatex("\\newpage \\textbf{\\LARGE{%s}}\n\n", header);    
}

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

static int  GetOpRank(OPER_TYPES operation)
{
    switch (operation)
    {
        case OP_POW:
            return 3;
        case OP_SIN:
        case OP_LOG:
        case OP_COS:
            return 2;
        
        case OP_DIV:
        case OP_MUL:
            return 1;
        
        case OP_PLUS:
        case OP_SUB:
            return 0;
        
        case UNDEF_OPER_TYPE:
        default:
            return -1;
    }
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
                  "\\bibitem{link2}  \"Сборник пословиц и поговорок под редацией кафедры высшей математики\"\n"
                  "\\bibitem{link3}  \"Полное собрание лучших высказываний преподавателей МФТИ\"\n"
                  "\\bibitem{link4}  \"Словарь фраз, не несущих смысловой нагрузки. 17 издание\"\n"
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

void PreFuncTexNode(Node* node, void* )
{
    bool print_bracket_L = PrintBracketL(node);
    
    if (IS_OP(node) && VAL_OP(node) == OP_DIV)
        PrintfInLatex("\\frac{")
    else if (print_bracket_L)
        PrintfInLatex("(");
}

void PrintElemInLatex(Node* node, void*)
{
    assert(node);

    if (IS_OP(node) && VAL_OP(node) == OP_DIV)
    {
        PrintfInLatex("}{")
        return;
    }
    
    bool print_bracket_L = PrintBracketL(node);

    if (print_bracket_L)
        PrintfInLatex(")");
        
    if (IS_OP(node) && VAL_OP(node) == OP_POW)
    {
        PrintfInLatex("^{");
        return;
    }

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
                PrintfInLatex("sin");
                break;
            
            case OP_COS:
                PrintfInLatex("cos");
                break;

            case OP_LOG:
                PrintfInLatex("log");
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

    bool print_bracket_R = PrintBracketR(node);
    if (print_bracket_R)
        PrintfInLatex("(");
}

void PostFuncTexNode(Node* node, void*)
{
    if (IS_OP(node) && (VAL_OP(node) == OP_DIV || VAL_OP(node) == OP_POW))
    {
        PrintfInLatex("}");
        return;
    }
    bool print_bracket_R = PrintBracketR(node);
    if (print_bracket_R)
        PrintfInLatex(")");
}

void TexNode(Node* root)
{
    assert(root);

    #ifdef DEBUG
        printf("Start tex node\n");
    #endif

    PrintfInLatex("$");

    DFS(root, PreFuncTexNode, nullptr, 
              PrintElemInLatex, nullptr, 
              PostFuncTexNode, nullptr);

    PrintfInLatex("$");
}

static bool PrintBracketL(Node* node)
{
    return (IS_L_NUM(node) && VAL_N(L(node)) < 0) ||
            (IS_OP(node) && IS_L_OP(node) && (GetOpRank(VAL_OP(node)) >= GetOpRank(VAL_OP(L(node)))));
}

static bool PrintBracketR(Node* node)
{
    return (IS_R_NUM(node) && VAL_N(R(node)) < 0)                                   ||                              
       (IS_OP(node) && (GetOpRank(VAL_OP(node)) == 2))                              ||
       (IS_R_OP(node) && GetOpRank(VAL_OP(node)) >= GetOpRank(VAL_OP(R(node)))  && 
                          VAL_OP(node) != OP_MUL && VAL_OP(node) != OP_PLUS);
}

void PrintElemDFS(FILE* stream, Node* node)
{
    if (node == nullptr)
        return;

    bool print_bracket_L = PrintBracketL(node);

    if (print_bracket_L)
        fprintf(stream, "(");

    PrintElemDFS(stream, L(node));

    if (print_bracket_L)
        fprintf(stream, ")");

    PrintElem(node, stream);

    bool print_bracket_R = PrintBracketR(node);

    if (print_bracket_R)
        fprintf(stream, "(");

    PrintElemDFS(stream, R(node));

    if (print_bracket_R)
        fprintf(stream, ")");
}

void PrintElem(Node* elem, FILE* stream)
{
    switch (TYPE(elem))
    {
    case TYPE_VAR:  
        fprintf(stream, "%s", VAL_VAR(elem));
        break;
    case TYPE_OP:
    {
        switch(VAL_OP(elem))
        {
            PUT_PLUS
            PUT_MUL
            PUT_SUB
            PUT_DIV
            PUT_SIN
            PUT_COS
            PUT_LOG
            PUT_POW

            case UNDEF_OPER_TYPE:
                fprintf(stream, "?");
                break;
            default:
                fprintf(stream, "#");
                break;
        }
        break;
    }
    case TYPE_NUM:
        fprintf(stream, "%lg", VAL_N(elem));
        break;
    case UNDEF_NODE_TYPE:
        fprintf(stream, "\n");
        break;
    default:
        fprintf(stream, "\t");
        break;
    }
}