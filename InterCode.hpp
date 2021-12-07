#ifndef CS323_PROJ3_INTERCODE_HPP
#define CS323_PROJ3_INTERCODE_HPP

#include <string>

using namespace std;

enum class OpType {
    PLACE,
    LABEL,
    VAR,
    IMMIDIATE,
    NAME
};

class Operand {
public:
    OpType type;
    string name;

    Operand(OpType type);

    Operand(OpType type, string name);

    string get_name();
};

enum class InterCodeType {
    LABEL,
    FUNCTION,
    ASSIGN,
    ADD, SUB, MUL, DIV,
    // 8 ~ 21
    READ, WRITE
};

class InterCode {
public:
    int interCodeType{}; // todo: change to enum
    Operand *x{};
    Operand *y{};
    Operand *z{};

    InterCode();

    InterCode(int type, Operand *x = nullptr, Operand *y = nullptr, Operand *z = nullptr);
};


#endif //CS323_PROJ3_INTERCODE_HPP
