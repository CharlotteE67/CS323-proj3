#ifndef CS323_PROJ3_INTERCODE_HPP
#define CS323_PROJ3_INTERCODE_HPP

#include <string>

using namespace std;

enum class OpType {
    PLACE,
    LABEL,
    VAR,
    IMMEDIATE,
    NAME,
    NONE
};

enum class AddrType {
    VALUE,
    ADDR,
    PTR
};

class Operand {
public:
    OpType type;
    string name;
    AddrType addr_type = AddrType::VALUE;

    Operand(OpType type);

    Operand(OpType type, string name);

    Operand(OpType type, AddrType addrType);

    string get_name();
};

enum class InterCodeType {
    NONE = 0,
    LABEL,
    FUNCTION,
    ASSIGN,
    ADD, SUB, MUL, DIV,
    // 8 ~ 21
    ADDR,
    PTR,
    COPY,
    GOTO,
    IF_S,
    IF_SE,
    IF_B,
    IF_BE,
    IF_NE,
    IF_EQ,
    RETURN,
    DEC,
    PARAM,
    ARG,
    CALL,
    READ, 
    WRITE
};

class InterCode {
public:
    InterCodeType interCodeType; // todo: change to enum
    Operand *x{};
    Operand *y{};
    Operand *z{};

    InterCode();

    InterCode(int type, Operand *x = nullptr, Operand *y = nullptr, Operand *z = nullptr);

    void print();
};


#endif //CS323_PROJ3_INTERCODE_HPP
