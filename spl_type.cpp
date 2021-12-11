#include "spl_type.hpp"

using namespace std;

Type::Type() {
    typePointer = nullptr;
}

Type::Type(string name, string pri): name(name), category(CATEGORY::PRIMITIVE) {
    if(pri == "int"){type.pri = Primitive::INT; size = 4;}
    else if(pri=="float"){type.pri = Primitive::FLOAT; size = 8;}
    else if(pri == "char"){type.pri = Primitive::CHAR; size = 2;}
}
Type::Type(string name, Array* arr): name(name), category(CATEGORY::ARRAY){
    type.arr = arr;
}

Type::Type(string name, Type *rt): name(name), category(CATEGORY::FUNCTION) {
    typePointer = rt;
}

Type::Type(string name, CATEGORY ca): name(name), category(ca){

}

int Type::getSize() {
    int sz = 0;
    switch (category) {

        case CATEGORY::PRIMITIVE:
        case CATEGORY::ARRAY:
        case CATEGORY::STRUCTURE:
            sz = size;
            break;
        case CATEGORY::STRUCTVAR:
            sz = get_structType()->getSize();
            break;
        case CATEGORY::FUNCTION:
            break;
    }
    return sz;
}

Type *Type::get_next_type() {
    return type.arr->base;
}

Array *Type::get_arrType() {
    return type.arr;
}

bool Type::is_addr_type() {
    return (category == CATEGORY::STRUCTURE || category == CATEGORY::ARRAY);
}

Array::Array(Type *base, int size) : base(base), size(size) {}

FieldList::FieldList() {type = nullptr; next = nullptr;}

FieldList::FieldList(string name, Type *type, FieldList *next) : name(std::move(name)), type(type), next(next) {}

