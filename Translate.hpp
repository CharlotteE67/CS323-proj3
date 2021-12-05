#ifndef CS323_PROJ3_TRANSLATE_HPP
#define CS323_PROJ3_TRANSLATE_HPP

#include "spl_node.hpp"
#include "InterCode.hpp"
#include <vector>
using namespace std;

string new_place();

string new_label();

vector<InterCode> translate_cond_Exp(Node *exp, string lt, string lf);

vector<InterCode> translate_Exp(Node *exp,string t);

vector<InterCode> translate_Stmt(Node *stmt);

#endif //CS323_PROJ3_TRANSLATE_HPP
