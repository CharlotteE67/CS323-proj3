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
                if (place->addr_type == AddrType::ADDR) {
                    ics.emplace_back(8, place, op);
                } else {
//                ics.emplace_back(3, place, op);
                    delete place;
                    place = op;
                }
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
            if ((exp->child[0]->get_name() == "Exp"||exp->child[0]->get_name()=="VarDec" )&& exp->child[1]->get_name() == "ASSIGN" &&
                exp->child[2]->get_name() == "Exp") {
                Operand *t1 = new_place();
                Operand *t2 = new_place();
                if (exp->child[0]->child.size() > 1)
                    t1->addr_type = AddrType::ADDR;
                ics = translate_Exp(exp->child[0], t1);
                // code1
                auto c1 = translate_Exp(exp->child[2], t2);
                ics.insert(ics.end(), c1.begin(), c1.end());
                // code2
                // code3
                if (t1->addr_type == AddrType::VALUE) {
                    ics.emplace_back(3, t1, t2);
                }
                else {// ADDR
                    ics.emplace_back(10, t1, t2);
                }
                ics.emplace_back(3, place, t1);
//                delete place;
//                place = t1;
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
                ics.emplace_back(4, place, t1, t2);
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
                ics.emplace_back(5, place, t1, t2);

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
                ics.emplace_back(6, place, t1, t2);

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
                ics.emplace_back(7, place, t1, t2);
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
            // Exp DOT ID
            else if (exp->child[0]->get_name() == "Exp" && exp->child[1]->get_name() == "DOT" &&
                      exp->child[2]->get_type() == Node_TYPE::ID) {
                auto imm = new_immediate(0);
                translate_offset(ics, exp, place, imm);
            }
            // LP Exp RP
            else if (exp->child[0]->get_name() == "LP" && exp->child[1]->get_name() == "Exp" &&
                     exp->child[2]->get_name() == "RP") {
                ics = translate_Exp(exp->child[1], place);
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
            // Exp [ Exp ]
            else if (exp->child[0]->get_name() == "Exp" && exp->child[1]->get_name() == "LB" &&
                     exp->child[2]->get_name() == "Exp" && exp->child[3]->get_name() == "RB") {
                auto imm = new_immediate(0);
                translate_offset(ics, exp, place, imm);
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
    // FOR LP Def Exp SEMI Exp RP Stmt
    else if (stmt->child.size() == 8) {
        // Def
        vector<InterCode> def = translate_Def(stmt->child[2]);
        translate.insert(translate.end(), def.begin(), def.end());
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
    // CompStmt
    else if (stmt->child.size() == 1){
        vector<InterCode> code =  merge_CompSt(stmt->child[0]);
        translate.insert(translate.end(), code.begin(), code.end());
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
        // Exp
        Operand *tp = new_place();
        if (args->child[0]->get_varType()->is_addr_type()) {
            tp->addr_type = AddrType::ADDR;
        }
        vector<InterCode> code = translate_Exp(args->child[0], tp);
        argList.push_back(tp);
        return code;
    } else {
        // Exp COMMA ARGS
        Operand *tp = new_place();
        if (args->child[0]->get_varType()->is_addr_type()) {
            tp->addr_type = AddrType::ADDR;
        }
        vector<InterCode> code1 = translate_Exp(args->child[0], tp);
        argList.push_back(tp);
        vector<InterCode> code2 = translate_Args(args->child[2], argList);
        code1.insert(code1.end(), code2.begin(), code2.end());
        return code1;
    }
}
/*  DefList -> Def DefList
        | $
    Def -> Specifier DecList SEMI
    DecList -> Dec
        | Dec COMMA DecList
    Dec -> VarDec
        | VarDec ASSIGN Exp
    VarDec -> ID
        | VarDec LB INT RB
*/
vector<InterCode> translate_arr(Node *defL){
    vector<InterCode> codes;
    if(defL->child.empty()){return codes;}
//    Node *def = defL->child[0];
    while(!defL->child.empty()){
        Node *def = defL->child[0];
        Node *decL = def->child[1];
        Node *dec = decL->child[0];
        while(true){
            if(dec->child.size()!=1){
                Operand *t = new Operand(OpType::NONE);
                vector<InterCode> tmp = translate_Exp(dec,t);
                codes.insert(codes.end(),tmp.begin(),tmp.end());
            }else{
                InterCode tmp = translate_arr_Dec(dec->child[0]);
                if(tmp.interCodeType!=InterCodeType::NONE){
                    codes.push_back(tmp);
                }
            }
            
            if(decL->child.size()==1){break;}
            decL = decL->child[2];
            dec = decL->child[0];
        }

//        if(defL->child.empty()){break;}
        defL = defL->child[1];
    }

    return codes;
}


InterCode translate_arr_Dec(Node *varDec){
    while(varDec->child.size() != 1){
        varDec = varDec->child[0];
    }
    string name = varDec->child[0]->get_name();
    if(varDec->child.size()==1) {
        Type *type = getTypeByName(name);
        if (type->category == CATEGORY::PRIMITIVE)
            return InterCode(0);
    }
    int size = getTypeByName(name)->getSize();
    Operand *x = get_varOp(name);
    Operand *y = new Operand(OpType::NAME,to_string(size));
    return InterCode(19,x,y);
}

// Exp -> Exp DOT Exp
// Exp -> Exp [ Exp ]
void translate_offset(vector<InterCode> &ics, Node *exp, Operand *&place, Operand *offset){
    switch (exp->child.size()) {
        case 1: {
            Operand *t1 = new_place();
            Operand *var = get_varOp(exp->child[0]->get_name());
            if (var->addr_type == AddrType::ADDR) {
                ics.emplace_back(3, t1, var);
            } else {
                ics.emplace_back(8, t1, var);
            }

            if (place->addr_type == AddrType::VALUE) {
                Operand *t2 = new_place(AddrType::ADDR);
                ics.emplace_back(4, t2, t1, offset);
                ics.emplace_back(9, place, t2);
            } else { // ADDR
                ics.emplace_back(4, place, t1, offset);
            }
            break;
        }
        case 3: {
            Type *st = exp->child[0]->get_varType();
            string id = exp->child[2]->get_name();
            Operand *ofs = new_immediate(get_struct_offset(st, id));
            Operand *base = new_place();
            ics.emplace_back(4, base, offset, ofs);
            translate_offset(ics, exp->child[0], place, base);
            break;
        }
        case 4: {
            Operand *base = new_place();
            Operand *ofs = new_place();
            auto c1 = translate_Exp(exp->child[2],ofs);
            ics.insert(ics.end(), c1.begin(), c1.end());
            ics.emplace_back(4, base, offset, ofs);
            translate_offset(ics, exp->child[0], place, base);
            break;
        }
    }
}

int get_struct_offset(Type *s, const string& id){
    int cnt = 0;
    FieldList *fl = s->get_fieldList();
    while (fl != nullptr) {
        if (fl->name == id)
            break;
        cnt += fl->type->getSize();
        fl = fl->next;
    }
    return cnt;
}

Operand *new_place(AddrType at) {
    switch (at) {
        case AddrType::VALUE:
            return new Operand(OpType::PLACE);
        case AddrType::ADDR:
            return new Operand(OpType::PLACE, at);
    }
}

Operand *new_label() {
    return new Operand(OpType::LABEL);
}

Operand *get_varOp(string var, AddrType at) {
    if (var_table.count(var)) return var_table[var];

    Operand *op;
    switch (at) {
        case AddrType::VALUE:
            op = new Operand(OpType::VAR);
            break;
        case AddrType::ADDR:
            op = new Operand(OpType::VAR, at);
            break;
    }
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
        if (fieldList->type->is_addr_type()) {
            var->addr_type = AddrType::ADDR;
        }
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
    return translate_arr(defList);
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




