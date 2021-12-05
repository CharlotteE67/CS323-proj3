#include "Translate.hpp"

int cnt_place = 0, cnt_label = 0, cnt_var = 0;

string new_place() {
    ++cnt_place;
    return "t" + to_string(cnt_place);
}

string new_label() {
    ++cnt_label;
    return "l" + to_string(cnt_label);
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