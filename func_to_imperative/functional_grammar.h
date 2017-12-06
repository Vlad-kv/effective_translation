#ifndef FUNCTIONAL_GRAMMAR_H
#define FUNCTIONAL_GRAMMAR_H

#include <vector>
#include <variant>
#include <memory>
using namespace std;

class f_type;

class real_term;

class term_seq;
class term;
class scope;
class var_name;

class let_definition;
class if_def;

typedef shared_ptr<f_type> f_type_sp;

typedef shared_ptr<term> term_sp;
typedef shared_ptr<let_definition> let_definition_sp;
typedef shared_ptr<if_def> if_def_sp;
typedef shared_ptr<term_seq> term_seq_sp;

#include "parser.h"

class f_type {
public:
    static const string DEFAULT_VAL;
    f_type(f_type_sp t_1, f_type_sp t_2, string val = DEFAULT_VAL);
    f_type(string str);
    
    static f_type_sp create(std::string str);
    friend bool operator==(const f_type_sp& type_1, const f_type_sp& type_2);
    friend bool operator!=(const f_type_sp& type_1, const f_type_sp& type_2);
//private:
    friend void loc_to_string(f_type_sp type, std::string& res, bool is_left);
    friend f_type_sp to_f_type(std::string input, int pos);
    
    f_type_sp t_1, t_2;
    std::string val;
};

string to_string(f_type_sp type);

class let_definition {
public:
//    let_definition(bool is_rec, string& name, vector<pair<string, f_type_sp>>&& arguments, f_type_sp ret_type, term_seq_sp terms)
//    : is_rec(is_rec), name(name), arguments(move(arguments)), ret_type(ret_type), terms(terms) {
//    }
    
    let_definition(bool is_rec, string& name, vector<string>&& arguments_names, f_type_sp raw_type, term_seq_sp terms)
    : is_rec(is_rec), name(name), terms(terms) {
        f_type_sp original_type = raw_type;
        for (string &w : arguments_names) {
            if (raw_type->val != f_type::DEFAULT_VAL) { // fatal error
                parser_errors.push_back("wrong number of arguments for type " + to_string(original_type));
                break;
            }
            arguments.push_back({w, raw_type->t_1});
            raw_type = raw_type->t_2;
        }
        ret_type = raw_type;
    }
    
    bool is_rec;
    string name;
    vector<pair<string, f_type_sp>> arguments;
    f_type_sp ret_type;
    term_seq_sp terms;
};

class term_seq {
public:
    term_seq() {
    }
    term_seq(vector<term_sp> &&terms)
    : terms(move(terms)) {
    }
    
    vector<term_sp> terms;
};

class scope {
public:
    scope(term_seq_sp terms)
    : terms(terms) {
    }
    
    term_seq_sp terms;
};

class if_def {
public:
    if_def(term_sp condition, term_sp if_branch, term_sp else_branch)
    : condition(condition), if_branch(if_branch), else_branch(else_branch) {
    }
    
    term_sp condition;
    term_sp if_branch;
    term_sp else_branch;
};

/* All except VAR_NAME. */
class real_term {
public:
    real_term(term_sp t_1, const string& val, term_sp t_2)
    : val(val), t_1(t_1), t_2(t_2) {
    }
    
    string val;
    term_sp t_1, t_2;
};
class var_name {
public:
    var_name(string &&name)
    : name(move(name)) {
    }
    string name;
};

class term {
public:
    term(real_term &&r_term, f_type_sp type)
    : data(r_term), type(type) {
    }
    
    term(let_definition_sp &&let_def, f_type_sp type)
    : data(let_def), type(type) {
    }
    
    term(if_def &&if_d, f_type_sp type)
    : data(if_d), type(type) {
    }
    
    term(scope &&sc, f_type_sp type)
    : data(sc), type(type) {
    }
    
    term(var_name &&var_n, f_type_sp type)
    : data(var_n), type(type) {
    }
    
    variant<real_term, let_definition_sp, if_def, scope, var_name> data;
    f_type_sp type;
};

string to_string(term_seq_sp term_s);

#endif // FUNCTIONAL_GRAMMAR_H
