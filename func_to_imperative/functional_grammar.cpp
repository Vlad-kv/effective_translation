#include "functional_grammar.h"
#include "parser.h"
#include <cassert>
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
    if (type == nullptr) {
        res += "nullptr";
        return;
    }
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

bool operator==(const f_type_sp& type_1, const f_type_sp& type_2) {
    if ((type_1 == nullptr) || (type_2 == nullptr)) {
        return ((type_1 == nullptr) && (type_2 == nullptr));
    }
    if (type_1->val != type_2->val) {
        return false;
    }
    return ((type_1->t_1 == type_2->t_1) && (type_1->t_2 == type_2->t_2));
}
bool operator!=(const f_type_sp& type_1, const f_type_sp& type_2) {
    return (!(type_1 == type_2));
}

std::string to_string(f_type_sp type) {
    string res;
    loc_to_string(type, res, false);
    return res;
}

/// *************************************************

string generate_spaces(int shift) {
    string res;
    for (int w = 0; w < shift; w++) {
        res += "    ";
    }
    return res;
}

void func_to_string(string &res, int shift, real_term &t);
void func_to_string(string &res, int shift, term_sp t);
void func_to_string(string &res, int shift, term_seq_sp term_s);
void func_to_string(string &res, int shift, let_definition_sp let_def);
void func_to_string(string &res, int shift, if_def &if_d);
void func_to_string(string &res, int shift, scope &sc);
void func_to_string(string &res, int shift, var_name &var_n);

void func_to_string(string &res, int shift, var_name &var_n) {
    res += generate_spaces(shift) + var_n.name;
}

void func_to_string(string &res, int shift, scope &sc) {
    string spaces = generate_spaces(shift);
    res += spaces + "begin\n";
    func_to_string(res, shift + 1, sc.terms);
    res += spaces + "end\n";
}

void func_to_string(string &res, int shift, real_term &t) {
    string spaces = generate_spaces(shift);
    if ((t.t_1 == nullptr) && (t.t_2 == nullptr)) {
        res += spaces + t.val;
        return;
    }
    if (t.t_1 == nullptr) {
        res += spaces + t.val + "\n";
        func_to_string(res, shift, t.t_2);
        return;
    }
    assert(t.t_2 != nullptr);
    res += spaces + "(\n";
    func_to_string(res, shift + 1, t.t_1);
    res += spaces + t.val + "\n";
    func_to_string(res, shift + 1, t.t_2);
    res += spaces + ")\n";
}

void func_to_string(string &res, int shift, let_definition_sp let_def) {
    string spaces = generate_spaces(shift);
    res += spaces + "let ";
    if (let_def->is_rec) {
        res += "rec ";
    }
    res += let_def->name + "\n";
    for (pair<string, f_type_sp> &w : let_def->arguments) {
        res += spaces + w.first + " : " + to_string(w.second) + "\n";
    }
    res += spaces + "=\n";
    func_to_string(res, shift + 1, let_def->terms);
    res += spaces + ": " + to_string(let_def->ret_type) + " in\n";
}

void func_to_string(string &res, int shift, if_def &if_d) {
    string spaces = generate_spaces(shift);
    res += spaces + "if\n";
    func_to_string(res, shift + 1, if_d.condition);
    res += spaces + "then\n";
    func_to_string(res, shift + 1, if_d.if_branch);
    
    if (if_d.else_branch != nullptr) {
        res += spaces + "else\n";
        func_to_string(res, shift + 1, if_d.else_branch);
    }
}

void func_to_string(string &res, int shift, term_sp t) {
    if (holds_alternative<real_term>(t->data)) {
        func_to_string(res, shift, get<real_term>(t->data));
    }
    if (holds_alternative<let_definition_sp>(t->data)) {
        func_to_string(res, shift, get<let_definition_sp>(t->data));
    }
    if (holds_alternative<if_def>(t->data)) {
        func_to_string(res, shift, get<if_def>(t->data));
    }
    if (holds_alternative<scope>(t->data)) {
        func_to_string(res, shift, get<scope>(t->data));
    }
    if (holds_alternative<var_name>(t->data)) {
        func_to_string(res, shift, get<var_name>(t->data));
    }
    
    if (res.back() == '\n') {
        res += generate_spaces(shift);
    }
    res += " : " + to_string(t->type) + "\n";
}

void func_to_string(string &res, int shift, term_seq_sp term_s) {
    for (term_sp &w : term_s->terms) {
        func_to_string(res, shift + 1, w);
    }
}

string to_string(term_seq_sp term_s) {
    string res;
    func_to_string(res, 0, term_s);
    return res;
}
