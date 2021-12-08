
#include "InterCode.hpp"

#include <utility>
#include <iostream>


extern int cnt_place, cnt_label, cnt_var;

InterCode::InterCode() = default;

string Operand::get_name() {
    string str;

    if (name.empty()) {
        switch (type) {
            case OpType::PLACE:
                name = to_string(++cnt_place);
                break;
            case OpType::LABEL:
                name = to_string(++cnt_label);
                break;
            case OpType::VAR:
                name = to_string(++cnt_var);
                break;
            default:
                break;
        }
    }

    switch (type) {
        case OpType::PLACE:
            str = "t" + name;
            break;
        case OpType::LABEL:
            str = "label" + name;
            break;
        case OpType::VAR:
            str = "v" + name;
            break;
        case OpType::IMMIDIATE:
            str = "#" + name;
            break;
        case OpType::NAME:
            str = name;
            break;
    }
    return str;
}

void InterCode::print() {
//    cout << (int)interCodeType << endl;
    switch (interCodeType) {
        case InterCodeType::LABEL:
            printf("LABEL %s\n", x->get_name().c_str());
            break;
        case InterCodeType::FUNCTION:
            printf("FUNCTION %s\n", x->get_name().c_str());
            break;
        case InterCodeType::ASSIGN:
            printf("%s := %s\n", x->get_name().c_str(), y->get_name().c_str());
            break;
        case InterCodeType::ADD:
            printf("%s := %s + %s\n", x->get_name().c_str(), y->get_name().c_str(), z->get_name().c_str());
            break;
        case InterCodeType::SUB:
            printf("%s := %s - %s\n", x->get_name().c_str(), y->get_name().c_str(), z->get_name().c_str());
            break;
        case InterCodeType::MUL:
            printf("%s := %s * %s\n", x->get_name().c_str(), y->get_name().c_str(), z->get_name().c_str());
            break;
        case InterCodeType::DIV:
            printf("%s := %s / %s\n", x->get_name().c_str(), y->get_name().c_str(), z->get_name().c_str());
            break;
        case InterCodeType::ADDR:
            printf("%s := &%s\n", x->get_name().c_str(), y->get_name().c_str());
            break;
        case InterCodeType::PTR:
            printf("%s := *%s\n", x->get_name().c_str(), y->get_name().c_str());
            break;
        case InterCodeType::COPY:
            printf("*%s := %s\n", x->get_name().c_str(), y->get_name().c_str());
            break;
        case InterCodeType::GOTO:
            printf("GOTO %s\n", x->get_name().c_str());
            break;
        case InterCodeType::IF_S:
            printf("IF %s < %s GOTO %s\n", x->get_name().c_str(), y->get_name().c_str(), z->get_name().c_str());
            break;
        case InterCodeType::IF_SE:
            printf("IF %s <= %s GOTO %s\n", x->get_name().c_str(), y->get_name().c_str(), z->get_name().c_str());
            break;
        case InterCodeType::IF_B:
            printf("IF %s > %s GOTO %s\n", x->get_name().c_str(), y->get_name().c_str(), z->get_name().c_str());
            break;
        case InterCodeType::IF_BE:
            printf("IF %s >= %s GOTO %s\n", x->get_name().c_str(), y->get_name().c_str(), z->get_name().c_str());
            break;
        case InterCodeType::IF_NE:
            printf("IF %s != %s GOTO %s\n", x->get_name().c_str(), y->get_name().c_str(), z->get_name().c_str());
            break;
        case InterCodeType::IF_EQ:
            printf("IF %s == %s GOTO %s\n", x->get_name().c_str(), y->get_name().c_str(), z->get_name().c_str());
            break;
        case InterCodeType::RETURN:
            printf("RETURN %s\n", x->get_name().c_str());
            break;
        case InterCodeType::DEC:
            printf("DEC %s %s\n", x->get_name().c_str(), y->get_name().c_str());
            break;
        case InterCodeType::PARAM:
            printf("PARAM %s\n", x->get_name().c_str());
            break;
        case InterCodeType::ARG:
            printf("ARG %s\n", x->get_name().c_str());
            break;
        case InterCodeType::CALL:
            printf("%s := CALL %s\n", x->get_name().c_str(), y->get_name().c_str());
            break;
        case InterCodeType::READ:
            printf("READ %s\n", x->get_name().c_str());
            break;
        case InterCodeType::WRITE:
            printf("WRITE %s\n", x->get_name().c_str());
            break;
    }
}

Operand::Operand(OpType type) : type(type) {}

Operand::Operand(OpType type, string name) : type(type), name(name) {}

InterCode::InterCode(int type, Operand *x, Operand *y, Operand *z) : interCodeType(static_cast<InterCodeType>(type)),
                                                                     x(x), y(y), z(z) {

}
