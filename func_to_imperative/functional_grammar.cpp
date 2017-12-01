#include "functional_grammar.h"
using namespace std;

const std::string f_type::DEFAULT_VAL = "->";

f_type::f_type(f_type_sp t_1, f_type_sp t_2, std::string val)
: t_1(t_1), t_2(t_2), val(val) {
}

void loc_to_string(f_type_sp type, std::string& res, bool is_left) {
    if (type->val != f_type::DEFAULT_VAL) {
        is_left = 0;
    }
    if (is_left) {
        res.push_back('(');
    }
    if ((type->t_1 != nullptr) && (type->t_2 != nullptr)) {
        loc_to_string(type->t_1, res, true);
        res.append(type->val);
        loc_to_string(type->t_2, res, false);
    } else {
        res.append(type->val);
    }
    if (is_left) {
        res.push_back(')');
    }
}

std::string to_string(f_type_sp type) {
    string res;
    loc_to_string(type, res, false);
    return res;
}
