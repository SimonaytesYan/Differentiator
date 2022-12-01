#ifndef __SYM_TAYLOR__
#define __SYM_TAYLOR__

#include "../TreeSetup.h"

Node* TaylorDecomp(Node* function, const char* variable, double var_value, unsigned int dec_order);

Node* GetFunctionValueAtPoint(Node* function, const char* variable, double var_value);

Node* GetOneTaylorDecMember(Node* derivative, int fact, int number_member,
                                   const char* variable, double var_value);

#endif //__SYM_TAYLOR__