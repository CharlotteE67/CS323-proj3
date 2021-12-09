#include "Translate.hpp"
#include <map>
#include <utility>
#include <string>
#include <deque>
#include "symbol_table.hpp"

using namespace std;
extern map<string, Type *> symbolTable;

int cnt_place = 0, cnt_label = 0, cnt_var = 0;
map<string, Operand *> var_table;

vector<InterCode> translate_Exp(Node *exp, Operand *&place) {

    vector<InterCode> ics;
    switch (exp->child.size()) {
        case 1:
            // INT
            if (exp->child[0]->get_type() == Node_TYPE::INT) {
                Operand *op = new_immediate(exp->child[0]->get_intVal());
//                ics.emplace_back(3, place, op);
                delete place;
                place = op;
            }
                // ID
            else if (exp->child[0]->get_type() == Node_TYPE::ID) {
                Operand *op = get_varOp(exp->child[0]->get_name());
//                ics.emplace_back(3, place, op);
                delete place;
                place = op;
            }
            break;
        case 2:
            // MINUS Exp
            if (exp->child[0]->get_name() == "MINUS" && exp->child[1]->get_name() == "Exp") {
                Operand *tp = new_place();
                Operand *zero = new_immediate(0);
                ics = translate_Exp(exp->child[1], tp);
                ics.emplace_back(5, place, zero, tp);
            }
                // NOT Exp
            else if (exp->child[0]->get_name() == "NOT" && exp->child[1]->get_name() == "Exp") {
                Operand *l1, *l2;
                l1 = new_label();
                l2 = new_label();
                // code0
                ics.emplace_back(3, place, new_immediate(0));
                // code1
                vector<InterCode> c1 = translate_cond_Exp(exp, l1, l2);
                ics.insert(ics.end(), c1.begin(), c1.end());
                // code2
                ics.emplace_back(InterCode(1, l1));
                ics.emplace_back(InterCode(3, place, new_immediate(1)));
                ics.emplace_back(InterCode(1, l2));
            }
            break;
        case 3:
            // 3: Exp ASSIGN Exp
            if (exp->child[0]->get_name() == "Exp" && exp->child[1]->get_name() == "ASSIGN" &&
                exp->child[2]->get_name() == "Exp") {
                // todo: translate assignable
                Operand *var = get_varOp(exp->child[0]->child[0]->get_name());
                Operand *tp = new_place();
                // code1
                ics = translate_Exp(exp->child[2], tp);
                // code2
                ics.emplace_back(3, var, tp);
                // code3
                ics.emplace_back(3, place, var);
//                delete place;
//                place = var;
            }
                // 4: Exp PLUS Exp
            else if (exp->child[0]->get_name() == "Exp" && exp->child[1]->get_name() == "PLUS" &&
                     exp->child[2]->get_name() == "Exp") {
                Operand *t1, *t2;
                t1 = new_place();
                t2 = new_place();
                // code1
                ics = translate_Exp(exp->child[0], t1);
                // code2
                vector<InterCode> c2 = translate_Exp(exp->child[2], t2);
                ics.insert(ics.end(), c2.begin(), c2.end());
                // code3
                ics.emplace_back(4, t1, t2);
            }
                // 5: Exp MINUS Exp
            else if (exp->child[0]->get_name() == "Exp" && exp->child[1]->get_name() == "MINUS" &&
                     exp->child[2]->get_name() == "Exp") {
                Operand *t1, *t2;
                t1 = new_place();
                t2 = new_place();
                // code1
                ics = translate_Exp(exp->child[0], t1);
                // code2
                vector<InterCode> c2 = translate_Exp(exp->child[2], t2);
                // code3
                ics.insert(ics.end(), c2.begin(), c2.end());
                ics.emplace_back(5, t1, t2);

            }
                // 6: Exp MUL Exp
            else if (exp->child[0]->get_name() == "Exp" && exp->child[1]->get_name() == "MUL" &&
                     exp->child[2]->get_name() == "Exp") {
                Operand *t1, *t2;
                t1 = new_place();
                t2 = new_place();
                // code1
                ics = translate_Exp(exp->child[0], t1);
                // code2
                vector<InterCode> c2 = translate_Exp(exp->child[2], t2);
                // code3
                ics.insert(ics.end(), c2.begin(), c2.end());
                ics.emplace_back(6, t1, t2);

            }
                // 7: Exp DIV Exp
            else if (exp->child[0]->get_name() == "Exp" && exp->child[1]->get_name() == "DIV" &&
                     exp->child[2]->get_name() == "Exp") {
                Operand *t1, *t2;
                t1 = new_place();
                t2 = new_place();
                // code1
                ics = translate_Exp(exp->child[0], t1);
                // code2
                vector<InterCode> c2 = translate_Exp(exp->child[2], t2);
                // code3
                ics.insert(ics.end(), c2.begin(), c2.end());
                ics.emplace_back(7, t1, t2);
            }

                // Exp cond. Exp
            else if (exp->child[0]->get_name() == "Exp"
                     && (exp->child[1]->get_name() == "LT" || exp->child[1]->get_name() == "LE" ||
                         exp->child[1]->get_name() == "GT" || exp->child[1]->get_name() == "GE" ||
                         exp->child[1]->get_name() == "NE" || exp->child[1]->get_name() == "EQ" ||
                         exp->child[1]->get_name() == "AND" || exp->child[1]->get_name() == "OR"
                     )
                     && exp->child[2]->get_name() == "Exp") {
                Operand *l1, *l2;
                l1 = new_label();
                l2 = new_label();
                // code0
                ics.emplace_back(3, place, new_immediate(0));
                // code1
                vector<InterCode> c1 = translate_cond_Exp(exp, l1, l2);
                ics.insert(ics.end(), c1.begin(), c1.end());
                // code2
                ics.emplace_back(InterCode(1, l1));
                ics.emplace_back(InterCode(3, place, new_immediate(1)));
                ics.emplace_back(InterCode(1, l2));
            }
                // ID LP RP
            else if (exp->child[0]->get_type() == Node_TYPE::ID && exp->child[1]->get_name() == "LP" &&
                     exp->child[2]->get_name() == "RP") {
                string func_name = exp->child[0]->get_name();
                if (func_name == "read") {
                    ics.emplace_back(23, place);
                } else {
                    auto *func = new Operand(OpType::NAME, func_name);
                    ics.emplace_back(22, place, func);
                }
            }
            break;
        case 4:
            // ID LP Args RP
            if (exp->child[0]->get_type() == Node_TYPE::ID && exp->child[1]->get_name() == "LP" &&
                exp->child[2]->get_name() == "Args" && exp->child[3]->get_name() == "RP") {
                string func_name = exp->child[0]->get_name();
                if (func_name == "write") {
                    Operand *tp = new_place();
                    ics = translate_Exp(exp->child[2]->child[0], tp);
                    ics.emplace_back(24, tp);
                } else {
                    auto *func = new Operand(OpType::NAME, func_name);
                    vector<Operand *> arg_list;
                    // code1
                    ics = translate_Args(exp->child[2], arg_list);
                    // code2
                    for (int i = arg_list.size() - 1; i >= 0; --i) {
                        ics.emplace_back(21, arg_list[i]);
                    }
                    // code3
                    ics.emplace_back(22, place, func);
                }
            }
            break;
    }
    return ics;
}


vector<InterCode> translate_Stmt(Node *stmt) {
    vector<InterCode> translate;
    // RETURN Exp SEMI
    if (stmt->child.size() == 3) {
        Operand *tp = new_place();
        vector<InterCode> codes = translate_Exp(stmt->child[1], tp);
        translate.insert(translate.end(), codes.begin(), codes.end());
        translate.emplace_back(18, tp); // RETURN
    }
        // IF LP Exp RP Stmt
    else if (stmt->child.size() == 5 && stmt->child[0]->get_name() == "IF") {
        Operand *lb1 = new_label();
        Operand *lb2 = new_label();
        vector<InterCode> code1 = translate_cond_Exp(stmt->child[2], lb1, lb2);
        vector<InterCode> code2 = translate_Stmt(stmt->child[4]);
        translate.insert(translate.end(), code1.begin(), code1.end());
        translate.emplace_back(1, lb1);
        translate.insert(translate.end(), code2.begin(), code2.end());
        translate.emplace_back(1, lb2);
    }
        // IF LP Exp RP Stmt_1 ELSE Stmt_2
    else if (stmt->child.size() == 7) {
        Operand *lb1 = new_label();
        Operand *lb2 = new_label();
        Operand *lb3 = new_label();
        vector<InterCode> code1 = translate_cond_Exp(stmt->child[2], lb1, lb2);
        vector<InterCode> code2 = translate_Stmt(stmt->child[4]);
        vector<InterCode> code3 = translate_Stmt(stmt->child[6]);
        translate.insert(translate.end(), code1.begin(), code1.end());
        translate.emplace_back(1, lb1);
        translate.insert(translate.end(), code2.begin(), code2.end());
        translate.emplace_back(11, lb3);
        translate.emplace_back(1, lb2);
        translate.insert(translate.end(), code3.begin(), code3.end());
        translate.emplace_back(1, lb3);
    }
        // WHILE LP Exp RP Stmt
    else if (stmt->child.size() == 5) {
        Operand *lb1 = new_label();
        Operand *lb2 = new_label();
        Operand *lb3 = new_label();
        vector<InterCode> code1 = translate_cond_Exp(stmt->child[2], lb2, lb3);
        vector<InterCode> code2 = translate_Stmt(stmt->child[4]);
        translate.emplace_back(1, lb1);
        translate.insert(translate.end(), code1.begin(), code1.end());
        translate.emplace_back(1, lb2);
        translate.insert(translate.end(), code2.begin(), code2.end());
        translate.emplace_back(11, lb1);
        translate.emplace_back(1, lb3);
    }
        // Exp SEMI
    else if (stmt->child.size() == 2) {
        auto *tp = new Operand(OpType::NONE);
        vector<InterCode> codes = translate_Exp(stmt->child[0], tp);
        translate.insert(translate.end(), codes.begin(), codes.end());
    }
        // CompSt
        /*
        Stmt -> CompSt
            -> LC DefList StmtList RC
                DefList -> null | Def DefList
                    Def -> Specifier DecList SEMI
                        Specifier -> TYPE(INT)
                        DecList -> Dec | Dec COMMA DecList
                            Dec -> VarDec | VarDec ASSIGN Exp
                                VarDec -> ID(INT)
                StmtList -> null | Stmt StmtList
        */
    else if (stmt->child.size() == 1) {
        // DefList
        Node *further_def = stmt->child[0]->child[1]; // DefList
        while (further_def->child.size() != 0) {
            Node *further_dec = further_def->child[0]->child[1]; // DecList
            while (further_dec->child.size() == 3) {
                if (further_dec->child[0]->child.size() == 1) { // VarDec
                    Operand *op = get_varOp(further_dec->child[0]->child[0]->child[0]->child[0]->get_name());
                } else { // VarDec ASSIGN Exp
                    // construct Exp -> Exp ASSIGN Exp
                    Node *id = further_dec->child[0]->child[0]->child[0];
                    Node *construct = new Node("Exp", id->get_lineNo());
                    vector<Node *> child = {id};
                    Node *exp_1 = new Node("Exp", id->get_lineNo(), child);
                    vector<Node *> childs = {exp_1, new Node("ASSIGN"), further_dec->child[0]->child[2]};
                    construct->set_child(childs);
                    Operand *tp = new_place();
                    vector<InterCode> assigns = translate_Exp(construct, tp);
                    translate.insert(translate.end(), assigns.begin(), assigns.end());
                }
                further_dec = further_dec->child[2];
            }
            if (further_dec->child[0]->child.size() == 1) { // VarDec
                Operand *op = get_varOp(further_dec->child[0]->child[0]->child[0]->child[0]->get_name());
            } else { // VarDec ASSIGN Exp
                // construct Exp -> Exp ASSIGN Exp
                Node *id = further_dec->child[0]->child[0]->child[0];
                Node *construct = new Node("Exp", id->get_lineNo());
                vector<Node *> child = {id};
                Node *exp_1 = new Node("Exp", id->get_lineNo(), child);
                vector<Node *> childs = {exp_1, new Node("ASSIGN"), further_dec->child[0]->child[2]};
                construct->set_child(childs);
                Operand *tp = new_place();
                vector<InterCode> assigns = translate_Exp(construct, tp);
                translate.insert(translate.end(), assigns.begin(), assigns.end());
            }
        }
        // StmtList
        Node *further_stmt = stmt->child[0]->child[2];
        while (further_stmt->child.size() != 0) {
            vector<InterCode> further_codes = translate_Stmt(further_stmt->child[0]);
            translate.insert(translate.end(), further_codes.begin(), further_codes.end());
            further_stmt = further_stmt->child[1];
        }
    }
        // FOR LP Def Exp SEMI Exp RP Stmt
    else if (stmt->child.size() == 8) {
        // Def
        /* Def -> Specifier DecList SEMI
            Specifier -> TYPE(INT)
            DecList -> Dec | Dec COMMA DecList
                Dec -> VarDec | VarDec ASSIGN Exp
                    VarDec -> ID(INT) */
        Node *decList = stmt->child[2]->child[1];
        while (decList->child.size() == 3) {
            if (decList->child[0]->child.size() == 1) { // VarDec
                Operand *op = get_varOp(decList->child[0]->child[0]->child[0]->child[0]->get_name());
            } else { // VarDec ASSIGN Exp
                // construct Exp -> Exp ASSIGN Exp
                Node *id = decList->child[0]->child[0]->child[0];
                Node *construct = new Node("Exp", id->get_lineNo());
                vector<Node *> child = {id};
                Node *exp_1 = new Node("Exp", id->get_lineNo(), child);
                vector<Node *> childs = {exp_1, new Node("ASSIGN"), decList->child[0]->child[2]};
                construct->set_child(childs);
                Operand *tp = new_place();
                vector<InterCode> assigns = translate_Exp(construct, tp);
                translate.insert(translate.end(), assigns.begin(), assigns.end());
            }
            decList = decList->child[2];
        }
        if (decList->child[0]->child.size() == 1) { // VarDec
            Operand *op = get_varOp(decList->child[0]->child[0]->child[0]->child[0]->get_name());
        } else { // VarDec ASSIGN Exp
            // construct Exp -> Exp ASSIGN Exp
            Node *id = decList->child[0]->child[0]->child[0];
            Node *construct = new Node("Exp", id->get_lineNo());
            vector<Node *> child = {id};
            Node *exp_1 = new Node("Exp", id->get_lineNo(), child);
            vector<Node *> childs = {exp_1, new Node("ASSIGN"), decList->child[0]->child[2]};
            construct->set_child(childs);
            Operand *tp = new_place();
            vector<InterCode> assigns = translate_Exp(construct, tp);
            translate.insert(translate.end(), assigns.begin(), assigns.end());
        }
        // WHILE Exp_2 Stmt + Exp_3
        Operand *lb1 = new_label();
        Operand *lb2 = new_label();
        Operand *lb3 = new_label();
        vector<InterCode> exp2 = translate_cond_Exp(stmt->child[4], lb2, lb3);
        vector<InterCode> code1 = translate_Stmt(stmt->child[8]);
        Operand *tp2 = new_place();
        vector<InterCode> code2 = translate_Exp(stmt->child[6], tp2);
        translate.emplace_back(1, lb1);
        translate.insert(translate.end(), exp2.begin(), exp2.end());
        translate.emplace_back(1, lb2);
        translate.insert(translate.end(), code1.begin(), code1.end());
        translate.insert(translate.end(), code2.begin(), code2.end());
        translate.emplace_back(11, lb1);
        translate.emplace_back(1, lb3);
    }
        // FOR LP Exp SEMI Exp SEMI Exp RP Stmt
    else if (stmt->child.size() == 9) {
        // Exp_1
        Operand *tp1 = new_place();
        vector<InterCode> exp1 = translate_Exp(stmt->child[0], tp1);
        translate.insert(translate.end(), exp1.begin(), exp1.end());
        // WHILE Exp_2 Stmt + Exp_3
        Operand *lb1 = new_label();
        Operand *lb2 = new_label();
        Operand *lb3 = new_label();
        vector<InterCode> exp2 = translate_cond_Exp(stmt->child[4], lb2, lb3);
        vector<InterCode> code1 = translate_Stmt(stmt->child[8]);
        Operand *tp2 = new_place();
        vector<InterCode> code2 = translate_Exp(stmt->child[6], tp2);
        translate.emplace_back(1, lb1);
        translate.insert(translate.end(), exp2.begin(), exp2.end());
        translate.emplace_back(1, lb2);
        translate.insert(translate.end(), code1.begin(), code1.end());
        translate.insert(translate.end(), code2.begin(), code2.end());
        translate.emplace_back(11, lb1);
        translate.emplace_back(1, lb3);
    }

    return translate;
}


vector<InterCode> translate_cond_Exp(Node *exp, Operand *lt, Operand *lf) {
    if (exp->child.size() == 2) {
        //NOT Exp
        return translate_cond_Exp(exp->child[1], lf, lt);
    } else if (exp->child[1]->get_name() == "LT") {
        //Exp LT Exp
        Operand *t1 = new_place();
        Operand *t2 = new_place();
        vector<InterCode> code1 = translate_Exp(exp->child[0], t1);
        vector<InterCode> code2 = translate_Exp(exp->child[2], t2);
        code1.insert(code1.end(), code2.begin(), code2.end());
        //IF == GOTO
        code1.emplace_back(12, t1, t2, lt);
        //GOTO LBF
        code1.emplace_back(11, lf);
        return code1;

    } else if (exp->child[1]->get_name() == "LE") {
        //Exp LE Exp
        Operand *t1 = new_place();
        Operand *t2 = new_place();
        vector<InterCode> code1 = translate_Exp(exp->child[0], t1);
        vector<InterCode> code2 = translate_Exp(exp->child[2], t2);
        code1.insert(code1.end(), code2.begin(), code2.end());
        //IF == GOTO
        code1.emplace_back(13, t1, t2, lt);
        //GOTO LBF
        code1.emplace_back(11, lf);
        return code1;

    } else if (exp->child[1]->get_name() == "GT") {
        //Exp GT Exp
        Operand *t1 = new_place();
        Operand *t2 = new_place();
        vector<InterCode> code1 = translate_Exp(exp->child[0], t1);
        vector<InterCode> code2 = translate_Exp(exp->child[2], t2);
        code1.insert(code1.end(), code2.begin(), code2.end());
        //IF == GOTO
        code1.emplace_back(14, t1, t2, lt);
        //GOTO LBF
        code1.emplace_back(11, lf);
        return code1;

    } else if (exp->child[1]->get_name() == "GE") {
        //Exp GE Exp
        Operand *t1 = new_place();
        Operand *t2 = new_place();
        vector<InterCode> code1 = translate_Exp(exp->child[0], t1);
        vector<InterCode> code2 = translate_Exp(exp->child[2], t2);
        code1.insert(code1.end(), code2.begin(), code2.end());
        //IF == GOTO
        code1.emplace_back(15, t1, t2, lt);
        //GOTO LBF
        code1.emplace_back(11, lf);
        return code1;

    } else if (exp->child[1]->get_name() == "NE") {
        //Exp NE Exp
        Operand *t1 = new_place();
        Operand *t2 = new_place();
        vector<InterCode> code1 = translate_Exp(exp->child[0], t1);
        vector<InterCode> code2 = translate_Exp(exp->child[2], t2);
        code1.insert(code1.end(), code2.begin(), code2.end());
        //IF == GOTO
        code1.emplace_back(16, t1, t2, lt);
        //GOTO LBF
        code1.emplace_back(11, lf);
        return code1;

    } else if (exp->child[1]->get_name() == "EQ") {
        //Exp EQ Exp
        Operand *t1 = new_place();
        Operand *t2 = new_place();
        vector<InterCode> code1 = translate_Exp(exp->child[0], t1);
        vector<InterCode> code2 = translate_Exp(exp->child[2], t2);
        code1.insert(code1.end(), code2.begin(), code2.end());
        //IF == GOTO
        code1.emplace_back(17, t1, t2, lt);
        //GOTO LBF
        code1.emplace_back(11, lf);
        return code1;

    } else if (exp->child[1]->get_name() == "AND") {
        Operand *lb1 = new_label();
        vector<InterCode> code1 = translate_cond_Exp(exp->child[0], lb1, lf);
        //+label bl1
        code1.emplace_back(InterCode(1, lb1));
        vector<InterCode> code2 = translate_cond_Exp(exp->child[2], lt, lf);
        code1.insert(code1.end(), code2.begin(), code2.end());
        return code1;
    } else if (exp->child[1]->get_name() == "OR") {
        Operand *lb1 = new_label();
        vector<InterCode> code1 = translate_cond_Exp(exp->child[0], lt, lb1);
        vector<InterCode> code2 = translate_cond_Exp(exp->child[2], lt, lf);
        code1.emplace_back(InterCode(1, lb1));
        code1.insert(code1.end(), code2.begin(), code2.end());
        return code1;
    }
}


vector<InterCode> translate_Args(Node *args, vector<Operand *> &argList) {
    if (args->child.size() == 1) {
        //Exp
        Operand *tp = new_place();
        vector<InterCode> code = translate_Exp(args->child[0], tp);
        argList.push_back(tp);
        return code;
    } else {
        //Exp COMMA ARGS
        Operand *tp = new_place();
        vector<InterCode> code1 = translate_Exp(args->child[0], tp);
        argList.push_back(tp);
        vector<InterCode> code2 = translate_Args(args->child[2], argList);
        code1.insert(code1.end(), code2.begin(), code2.end());
        return code1;
    }
}

InterCode translate_arr_Dec(Node *varDec){
    if(varDec->child.size()==1){return {};}
    while(varDec->child.size()!=1){
        varDec = varDec->child[0];
    }
    string name = varDec->get_name();
    int size = get_arr_size(name);
    Operand *x = get_varOp(name);
    Operand *y = new Operand(OpType::IMMIDIATE,to_string(size));
    return InterCode(19,x,y);
}

Operand *new_place() {
    return new Operand(OpType::PLACE);
}

Operand *new_label() {
    return new Operand(OpType::LABEL);
}

Operand *get_varOp(string var) {
    if (var_table.count(var)) return var_table[var];
    auto *op = new Operand(OpType::VAR);
    var_table[var] = op;
    return op;
}

Operand *new_immediate(int i) {
    return new Operand(OpType::IMMEDIATE, to_string(i));
}

vector<InterCode> translate_FunDec(Node *funDec) {
    vector<InterCode> ics;
    string func_name = funDec->child[0]->get_name();
    ics.emplace_back(2, new Operand(OpType::NAME, func_name));
    if (funDec->child.size() == 4) {
        FieldList *fieldList = symbolTable[func_name]->get_fieldList();
        Operand *var = get_varOp(fieldList->name);
        ics.emplace_back(20, var);
    }
    return ics;
}

vector<InterCode> translate_Def(Node *Def) {
    return vector<InterCode>();
}

// ExtDef: Specifier FunDec CompSt
vector<InterCode> merge_ExtDef(Node *extDef) {
    vector<InterCode> ics;
    auto funDec_codes = translate_FunDec(extDef->child[1]);
    auto compSt_codes = merge_CompSt(extDef->child[2]);
    ics.insert(ics.end(), funDec_codes.begin(), funDec_codes.end());
    ics.insert(ics.end(), compSt_codes.begin(), compSt_codes.end());
    return ics;
}

// LC DefList StmtList RC
vector<InterCode> merge_CompSt(Node *compSt) {
    vector<InterCode> ics;
    auto defList_codes = merge_DefList(compSt->child[1]);
    auto stmtList_codes = merge_StmtList(compSt->child[2]);
    ics.insert(ics.end(), defList_codes.begin(), defList_codes.end());
    ics.insert(ics.end(), stmtList_codes.begin(), stmtList_codes.end());
    return ics;
}

// null | Def DefList
vector<InterCode> merge_DefList(Node *defList) {
    Node *p = defList;
    vector<InterCode> ics;
    while (!p->child.empty()) {
        auto child_codes = translate_Def(p->child[0]);
        ics.insert(ics.end(), child_codes.begin(), child_codes.end());
        p = p->child[1];
    }
    return ics;
}

// null | Stmt StmtList
vector<InterCode> merge_StmtList(Node *stmtList) {
    Node *p = stmtList;
    vector<InterCode> ics;
    while (!p->child.empty()) {
        auto child_codes = translate_Stmt(p->child[0]);
        ics.insert(ics.end(), child_codes.begin(), child_codes.end());
        p = p->child[1];
    }
    return ics;
}

void print_codes(vector<InterCode> &ics) {
    for (auto & ic : ics) {
        ic.print();
    }
}




