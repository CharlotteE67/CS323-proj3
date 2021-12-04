#ifndef CS323_PROJ3_INTERCODE_HPP
#define CS323_PROJ3_INTERCODE_HPP
#include <string>
using namespace std;


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
    int interCodeType; // todo: change to enum
    string x;
    string y;
    string z;

    InterCode();
};


#endif //CS323_PROJ3_INTERCODE_HPP
