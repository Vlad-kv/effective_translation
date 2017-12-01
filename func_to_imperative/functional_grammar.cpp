#include "functional_grammar.h"
using namespace std;

const std::string f_type::DEFAULT_VAL = "->";

f_type::f_type(f_type_sp t_1, f_type_sp t_2, std::string val)
: t_1(t_1), t_2(t_2), val(val) {
}

void skip_spaces(string &input, size_t &pos) {
    while (input[pos] == ' ') {
        pos++;
    }
}
string extract_type_name(string &input, size_t &pos) {
    string res;
    skip_spaces(input, pos);
    char c = input[pos];
    while ((c != 0) && (c != ' ') && (c != '(') && (c != ')') && (c != '-')) {
        res.push_back(c);
        pos++;
        c = input[pos];
    }
    if (res.size() == 0) {
        throw runtime_error("no type name at position " + to_string(pos) +
                            " in " + input);
    }
    skip_spaces(input, pos);
    return res;
}
f_type_sp to_f_type(std::string &input, size_t &pos) {
    skip_spaces(input, pos);
    if (input[pos] == '(') {
        pos++;
        f_type_sp res = to_f_type(input, pos);
        skip_spaces(input, pos);
        if (input[pos] != ')') {
            throw runtime_error("no closing brace on position " + to_string(pos));
        }
        pos++;
        return res;
    }
    string type = extract_type_name(input, pos);
    f_type_sp res_1 = make_shared<f_type>(nullptr, nullptr, type);
    char c = input[pos];
    if ((c == '(') || (c == ')') || (c == 0)) {
        return res_1;
    }
    if ((input.size() - pos >= 2) && (input.substr(pos, 2) == "->")) {
        pos += 2;
        f_type_sp res_2 = to_f_type(input, pos);
        return make_shared<f_type>(res_1, res_2);
    }
    throw runtime_error("unexpected symbol on position " + to_string(pos)
                        + " in " + input);
}

f_type_sp f_type::create(std::string str) {
    size_t pos = 0;
    f_type_sp res = to_f_type(str, pos);
    skip_spaces(str, pos);
    if (pos != str.size()) {
        throw runtime_error("error on position " + to_string(pos)
                            + " in " + str);
    }
    return res;
}

f_type::f_type(std::string str) {
    f_type_sp res = create(str);
    this->t_1 = res->t_1;
    this->t_2 = res->t_2;
    this->val = res->val;
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
