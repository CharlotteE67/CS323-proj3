#include "Translate.hpp"
#include <string>

int cnt_place = 0, cnt_label = 0, cnt_var = 0;

string new_place() {
    ++cnt_place;
    return "t" + to_string(cnt_place);
}

string new_label() {
    ++cnt_label;
    return "l" + to_string(cnt_label);
}


vector<InterCode> translate_Stmt(Node *stmt) {
    if (stmt->get_name() != "stmt")
        return;
    vector <InterCode> translate;
    // RETURN Exp SEMI
    if (stmt->child.size() == 3) {
        string tp = new_place();
        vector<InterCode> codes = translate_Exp(stmt->child[1], tp);
        InterCode return_code = new InterCode();
        translate.insert(translate.end(), codes.begin(), codes.end());
        translate.push_back(new InterCode(18, tp)); // RETURN
    }
    // IF LP Exp RP Stmt
    else if (stmt->child.size() == 5 && stmt->child[0].get_name() == "IF"){
        string lb1 = new_label();
        string lb2 = new_label();
        vector<InterCode> code1 = translate_cond_Exp(stmt->child[2], lb1, lb2);
        vector<InterCode> code2 = translate_Stmt(stmt->child[4]);
        translate.insert(translate.end(), code1.begin(), code1.end());
        translate.push_back(new InterCode(1, lb1));
        translate.insert(translate.end(), code2.begin(), code2.end());
        translate.push_back(new InterCode(1, lb2));
    }
    // IF LP Exp RP Stmt_1 ELSE Stmt_2
    else if (stmt->child.size() == 7){
        string lb1 = new_label();
        string lb2 = new_label();
        string lb3 = new_label();
        vector<InterCode> code1 = translate_cond_Exp(stmt->child[2], lb1, lb2);
        vector<InterCode> code2 = translate_Stmt(stmt->child[4]);
        vector<InterCode> code3 = translate_Stmt(stmt->child[6]);
        translate.insert(translate.end(), code1.begin(), code1.end());
        translate.push_back(new InterCode(1, lb1));
        translate.insert(translate.end(), code2.begin(), code2.end());
        translate.push_back(new InterCode(11, lb3));
        translate.push_back(new InterCode(1, lb2));
        translate.insert(translate.end(), code3.begin(), code3.end());
        translate.push_back(new InterCode(1, lb3));
    }
    // WHILE LP Exp RP Stmt
    else if (stmt->child.size() == 5){
        string lb1 = new_label();
        string lb2 = new_label();
        string lb3 = new_label();
        vector<InterCode> code1 = translate_cond_Exp(stmt->child[2], lb2, lb3);
        vector<InterCode> code2 = translate_Stmt(stmt->child[4]);
        translate.push_back(new InterCode(1, lb1));
        translate.insert(translate.end(), code1.begin(), code1.end());
        translate.push_back(new InterCode(1, lb2));
        translate.insert(translate.end(), code2.begin(), code2.end());
        translate.push_back(new InterCode(11, lb1));
        translate.push_back(new InterCode(1, lb3));
    }
    // Exp SEMI
    else if (stmt->child.size() == 2){
        string tp = new_place();
        vector<InterCode> codes = translate_Exp(stmt->child[0], tp);
        translate.insert(translate.end(), codes.begin(), codes.end());
    }
    // CompSt
    else if (stmt->child.size() == 1){ }
    // FOR LP Def Exp SEMI Exp RP Stmt
    else if (stmt->child.size() == 8) { }
    // FOR LP Exp SEMI Exp SEMI Exp RP Stmt
    else if (stmt->child.size() == 9){
        string tp1 = new_place();
        string tp2 = new_place();
        string lb1 = new_label();
        string lb2 = new_label();
        vector<InterCode> code1 = translate_Exp(stmt->child[2], tp1);
        vector<InterCode> code2 = translate_cond_Exp(stmt->child[4], lb1, lb2);
        vector<InterCode> code3 = translate_Exp(stmt->child[5], tp2);
    }

    return translate;
}


vector<InterCode> translate_cond_Exp(Node *exp, string lt, string lf){
    if(exp->child.size()==2){
        //NOT EXP
        return translate_cond_Exp(exp->child[1],lf,lt);
    }else if(exp->child[1]->get_name()=="EQ"){
        //EXP EQ EXP
        string t1 = new_place();
        string t2 = new_place();
        vector<InterCode> code1 = translate_Exp(exp->child[0],t1);
        vector<InterCode> code2 = translate_Exp(exp->child[2],t2);
        code1.insert(code1.end(),code2.begin(),code2.end());
        //IF == GOTO
        code1.emplace_back(17,t1,t2,lt);
        //GOTO LBF
        code1.emplace_back(11,lf);
        return code1;

    }else if(exp->child[1]->get_name()=="AND"){
        string lb1 = new_label();
        vector<InterCode> code1 = translate_cond_Exp(exp->child[0],lb1,lf);
        //+label bl1
        code1.emplace_back(InterCode(1,lb1));
        vector<InterCode> code2 = translate_cond_Exp(exp->child[2],lt,lf);
        code1.insert(code1.end(),code2.begin(),code2.end());
        return code1;
    }else if(exp->child[1]->get_name()=="OR"){
        string lb1 = new_label();
        vector<InterCode> code1 = translate_cond_Exp(exp->child[0],lt,lb1);
        vector<InterCode> code2 = translate_cond_Exp(exp->child[2],lt,lf);
        code1.emplace_back(InterCode(1,lb1));
        code1.insert(code1.end(),code2.begin(),code2.end());
        return code1;
    }
}