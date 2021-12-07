#include "Translate.hpp"
#include <map>
#include <utility>

using namespace std;
#include <string>

int cnt_place = 0, cnt_label = 0, cnt_var = 0;
map<string, Operand *> var_table;

vector<InterCode> translate_Exp(Node *exp, Operand *place) {

    vector<InterCode> ics;
    switch (exp->child.size()) {
        case 1:
            // INT
            if (exp->child[0]->get_type() == Node_TYPE::INT) {
                Operand *op = new_immidiate(exp->child[0]->get_intVal());
                ics.emplace_back(3, place, op);
            }
                // ID
            else if (exp->child[0]->get_type() == Node_TYPE::ID) {
                Operand *op = get_varOp(exp->child[0]->get_name());
                ics.emplace_back(3, place, op);
            }
            break;
        case 2:
            // MINUS EXP
            if (exp->child[0]->get_name() == "MINUS" && exp->child[1]->get_name() == "EXP") {
                Operand *tp = new_place();
                Operand *zero = new_immidiate(0);
                ics = translate_Exp(exp, tp);
                ics.emplace_back(5, place, zero, tp);
            }
                // todo: NOT EXP
            else if (exp->child[0]->get_name() == "NOT" && exp->child[1]->get_name() == "EXP") {
                Operand *l1, *l2;
                l1 = new_label();
                l2 = new_label();
                // code0
                ics.emplace_back(3, place, new_immidiate(0));
                // code1
                vector<InterCode> c1 = translate_cond_Exp(exp, l1, l2);
                ics.insert(ics.end(), c1.begin(), c1.end());
                // code2
                ics.emplace_back(InterCode(1, l1));
                ics.emplace_back(InterCode(3, place, new_immidiate(1)));
                ics.emplace_back(InterCode(1, l2));
            }
            break;
        case 3:
            // todo: 3: EXP ASSIGN EXP
            if (exp->child[0]->get_name() == "EXP" && exp->child[1]->get_name() == "ASSIGN" &&
                exp->child[2]->get_name() == "EXP") {

            }
                // todo: 4: EXP PLUS EXP
            else if (exp->child[0]->get_name() == "EXP" && exp->child[1]->get_name() == "PLUS" &&
                     exp->child[2]->get_name() == "EXP") {

            }
                // todo: 5: EXP MINUS EXP
            else if (exp->child[0]->get_name() == "EXP" && exp->child[1]->get_name() == "MINUS" &&
                     exp->child[2]->get_name() == "EXP") {

            }
                // todo: 6: EXP MUL EXP
            else if (exp->child[0]->get_name() == "EXP" && exp->child[1]->get_name() == "MUL" &&
                     exp->child[2]->get_name() == "EXP") {

            }
                // todo: 7: EXP DIV EXP
            else if (exp->child[0]->get_name() == "EXP" && exp->child[1]->get_name() == "DIV" &&
                     exp->child[2]->get_name() == "EXP") {

            }

                // EXP cond. Exp
            else if (exp->child[0]->get_name() == "EXP"
                     && (exp->child[1]->get_name() == "LT" || exp->child[1]->get_name() == "LE" ||
                         exp->child[1]->get_name() == "GT" || exp->child[1]->get_name() == "GE" ||
                         exp->child[1]->get_name() == "NE" || exp->child[1]->get_name() == "EQ" ||
                         exp->child[1]->get_name() == "AND" || exp->child[1]->get_name() == "OR"
                     )
                     && exp->child[2]->get_name() == "EXP") {
                Operand *l1, *l2;
                l1 = new_label();
                l2 = new_label();
                // code0
                ics.emplace_back(3, place, new_immidiate(0));
                // code1
                vector<InterCode> c1 = translate_cond_Exp(exp, l1, l2);
                ics.insert(ics.end(), c1.begin(), c1.end());
                // code2
                ics.emplace_back(InterCode(1, l1));
                ics.emplace_back(InterCode(3, place, new_immidiate(1)));
                ics.emplace_back(InterCode(1, l2));
            }
            break;
    }
    return ics;
}


vector<InterCode> translate_Stmt(Node *stmt) {
    vector <InterCode> translate;
    // RETURN Exp SEMI
    if (stmt->child.size() == 3) {
        Operand* tp = new_place();
        vector<InterCode> codes = translate_Exp(stmt->child[1], tp);
        translate.insert(translate.end(), codes.begin(), codes.end());
        translate.emplace_back(18, tp); // RETURN
    }
    // IF LP Exp RP Stmt
    else if (stmt->child.size() == 5 && stmt->child[0]->get_name() == "IF"){
        Operand* lb1 = new_label();
        Operand* lb2 = new_label();
        vector<InterCode> code1 = translate_cond_Exp(stmt->child[2], lb1, lb2);
        vector<InterCode> code2 = translate_Stmt(stmt->child[4]);
        translate.insert(translate.end(), code1.begin(), code1.end());
        translate.emplace_back(1, lb1);
        translate.insert(translate.end(), code2.begin(), code2.end());
        translate.emplace_back(1, lb2);
    }
    // IF LP Exp RP Stmt_1 ELSE Stmt_2
    else if (stmt->child.size() == 7){
        Operand* lb1 = new_label();
        Operand* lb2 = new_label();
        Operand* lb3 = new_label();
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
    else if (stmt->child.size() == 5){
        Operand* lb1 = new_label();
        Operand* lb2 = new_label();
        Operand* lb3 = new_label();
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
    else if (stmt->child.size() == 2){
        Operand* tp = new_place();
        vector<InterCode> codes = translate_Exp(stmt->child[0], tp);
        translate.insert(translate.end(), codes.begin(), codes.end());
    }
    // CompSt
    else if (stmt->child.size() == 1){ }
    // FOR LP Def Exp SEMI Exp RP Stmt
    else if (stmt->child.size() == 8) { }
    // FOR LP Exp SEMI Exp SEMI Exp RP Stmt
    else if (stmt->child.size() == 9){
        // Exp_1
        Operand* tp1 = new_place();
        vector<InterCode> exp1 = translate_Exp(stmt->child[0], tp1);
        translate.insert(translate.end(), exp1.begin(), exp1.end());
        // WHILE Exp_2 Stmt + Exp_3
        Operand* lb1 = new_label();
        Operand* lb2 = new_label();
        Operand* lb3 = new_label();
        vector<InterCode> exp2 = translate_cond_Exp(stmt->child[4], lb2, lb3);
        vector<InterCode> code1 = translate_Stmt(stmt->child[8]);
        Operand* tp2 = new_place();
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


vector<InterCode> translate_cond_Exp(Node *exp, Operand* lt, Operand* lf){
    if(exp->child.size()==2){
        //NOT EXP
        return translate_cond_Exp(exp->child[1],lf,lt);
    }else if(exp->child[1]->get_name()=="EQ"){
        //EXP EQ EXP
        Operand* t1 = new_place();
        Operand* t2 = new_place();
        vector<InterCode> code1 = translate_Exp(exp->child[0],t1);
        vector<InterCode> code2 = translate_Exp(exp->child[2],t2);
        code1.insert(code1.end(),code2.begin(),code2.end());
        //IF == GOTO
        code1.emplace_back(InterCode(17,t1,t2,lt));
        //GOTO LBF
        code1.emplace_back(InterCode(11,lf));
        return code1;

    }else if(exp->child[1]->get_name()=="AND"){
        Operand* lb1 = new_label();
        vector<InterCode> code1 = translate_cond_Exp(exp->child[0],lb1,lf);
        //+label bl1
        code1.emplace_back(InterCode(1,lb1));
        vector<InterCode> code2 = translate_cond_Exp(exp->child[2],lt,lf);
        code1.insert(code1.end(),code2.begin(),code2.end());
        return code1;
    }else if(exp->child[1]->get_name()=="OR"){
        Operand* lb1 = new_label();
        vector<InterCode> code1 = translate_cond_Exp(exp->child[0],lt,lb1);
        vector<InterCode> code2 = translate_cond_Exp(exp->child[2],lt,lf);
        code1.emplace_back(InterCode(1,lb1));
        code1.insert(code1.end(),code2.begin(),code2.end());
        return code1;
    }
}


vector<InterCode> translate_Args(Node *args,vector<Operand *>argList){
    if(args->child.size()==1){
        //EXP   
        Operand *tp = new_place();
        vector<InterCode> code = translate_Exp(args->child[0],tp);
        argList.push_back(tp);
        return code;
    }else{
        //EXP COMMA ARGS
        Operand *tp = new_place();
        vector<InterCode> code1 = translate_Exp(args->child[0],tp);
        argList.push_back(tp);
        vector<InterCode> code2 = translate_Args(args->child[2],argList);
        code1.insert(code1.end(),code2.begin(),code2.end());
        return code1;
    }
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

Operand *new_immidiate(int i) {
    return new Operand(OpType::IMMIDIATE, to_string(i));
}


