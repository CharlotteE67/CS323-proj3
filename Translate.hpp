#ifndef CS323_PROJ3_TRANSLATE_HPP
#define CS323_PROJ3_TRANSLATE_HPP

#include "spl_node.hpp"
#include "InterCode.hpp"
#include "symbol_table.hpp"
#include <vector>

using namespace std;

Operand *new_place(AddrType at = AddrType::VALUE);

Operand *new_label();

Operand *new_immediate(int i);

Operand *get_varOp(string var, AddrType at = AddrType::VALUE);

vector<InterCode> translate_cond_Exp(Node *exp, Operand *l1, Operand *l2);

vector<InterCode> translate_Exp(Node *exp, Operand *&place);

vector<InterCode> translate_Stmt(Node *stmt);

vector<InterCode> translate_Args(Node *args, vector<Operand *> &argList);

vector<InterCode> translate_arr(Node *defL);

InterCode translate_arr_Dec(Node *varDec);

vector<InterCode> translate_FunDec(Node *funDec);

vector<InterCode> merge_ExtDef(Node *extDef);

vector<InterCode> merge_CompSt(Node *compSt);

vector<InterCode> merge_DefList(Node *defList);

vector<InterCode> merge_StmtList(Node *stmtList);

vector<InterCode> translate_Def(Node *Def);

void print_codes(vector<InterCode> &ics);

void translate_offset(vector<InterCode> &ics, Node *exp, Operand *&place, Operand *offset);

int get_struct_offset(Type *s, const string& id);

#endif //CS323_PROJ3_TRANSLATE_HPP
