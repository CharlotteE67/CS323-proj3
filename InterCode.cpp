
#include "InterCode.hpp"

#include <utility>

InterCode::InterCode() = default;

string Operand::get_name() {
    string str;
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

Operand::Operand(OpType type) : type(type) {}

Operand::Operand(OpType type, string name) : type(type), name(name) {}

InterCode::InterCode(int type, Operand *x, Operand *y, Operand *z): x(x), y(y), z(z) {

}
