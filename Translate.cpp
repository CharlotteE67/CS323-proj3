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