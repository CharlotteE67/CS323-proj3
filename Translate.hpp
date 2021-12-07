#ifndef CS323_PROJ3_TRANSLATE_HPP
#define CS323_PROJ3_TRANSLATE_HPP

#include "spl_node.hpp"
#include "InterCode.hpp"
#include <vector>
using namespace std;

Operand* new_place();

Operand* new_label();

Operand* new_immidiate(int i);

Operand* get_varOp(string var);

vector<InterCode> translate_cond_Exp(Node *exp, Operand* l1, Operand* l2);

vector<InterCode> translate_Exp(Node *exp, Operand* place);

vector<InterCode> translate_Stmt(Node *stmt);

#endif //CS323_PROJ3_TRANSLATE_HPP
