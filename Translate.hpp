#ifndef CS323_PROJ3_TRANSLATE_HPP
#define CS323_PROJ3_TRANSLATE_HPP

#include "spl_node.hpp"
#include "InterCode.hpp"
#include <vector>

using namespace std;

Operand *new_place();

Operand *new_label();

Operand *new_immidiate(int i);

Operand *get_varOp(string var);

vector<InterCode> translate_cond_Exp(Node *exp, Operand *l1, Operand *l2);

vector<InterCode> translate_Exp(Node *exp, Operand *&place);

vector<InterCode> translate_Stmt(Node *stmt);

vector<InterCode> translate_Args(Node *args, vector<Operand *> &argList);

vector<InterCode> translate_FunDec(Node *funDec);

void merge_CompSt(Node *compSt);

void merge_DefList(Node *defList);

void merge_StmtList(Node *stmtList);

vector<InterCode> translate_Def(Node *Def);

#endif //CS323_PROJ3_TRANSLATE_HPP
