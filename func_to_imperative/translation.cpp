#include <map>
#include <set>
#include <string>
#include <cassert>

#include "translation.h"
#include "parser.h"
#include "preparation.h"

using namespace std;
term_seq_sp global_result = nullptr;

namespace {
    set<string> visible_vars;
    
    struct var_holder {
        var_holder() {
        }
        void insert(string var) {
            vars.push_back(var);
            visible_vars.insert(var);
        }
        var_holder& operator = (const var_holder& h) = delete; 
        ~var_holder() {
            for (string &w : vars) {
                visible_vars.erase(w);
            }
        }
    private:
        vector<string> vars;
    };
}

string to_cpp_type(f_type_sp t) {
    if (t->val != f_type::DEFAULT_VAL) {
        return t->val;
    }
    string arguments;
    while (t->val == f_type::DEFAULT_VAL) {
        if (arguments != "") {
            arguments += ", ";
        }
        arguments += to_cpp_type(t->t_1);
        t = t->t_2;
    }
    return "function<" + t->val + " (" + arguments + ")>";
}

void add_code(source_code_t &res, source_code_t &&to_add) {
    if (to_add.size() == 1) {
        res.back().first += to_add.front().first;
    } else {
        res.back().second++;
        res.splice(res.end(), to_add);
        res.back().second--;
    }
}
void add_code(source_code_t &res, string str) {
    if (res.back().second == 0) {
        res.back().first += str;
    } else {
        res.push_back({str, 0});
    }
}

source_code_t translate(term_seq_sp terms, bool to_return, const string &new_args_to_call = "");
source_code_t translate(term_sp t, var_holder &holder);

source_code_t translate(if_def &if_d, var_holder &holder) {
    source_code_t res;
    if (if_d.if_branch->type == f_type::create("unit")) {
        res.push_back({"if (", 0});
        add_code(res, translate(if_d.condition, holder));
        add_code(res, ") {");
        res.back().second++;
        
        res.splice(res.end(), translate(if_d.if_branch, holder));
        
        res.back().first.push_back(';');
        res.back().second--;
        res.push_back({"} else {", 1});
        
        res.splice(res.end(), translate(if_d.else_branch, holder));
        
        res.back().first.push_back(';');
        res.back().second--;
        res.push_back({"}", 0});
    } else {
        res.push_back({"(", 0});
        add_code(res, translate(if_d.condition, holder));
        add_code(res, ")?");
        res.back().second++;
        
        res.splice(res.end(), translate(if_d.if_branch, holder));
        
        res.back().second--;
        res.push_back({":", 1});
        
        res.splice(res.end(), translate(if_d.else_branch, holder));
        res.back().second--;
    }
    return res;
}

source_code_t translate(var_name &var_n, var_holder &holder) {
    holder.insert(var_n.name);
    return {{var_n.name, 0}};
}

source_code_t translate(scope &sc, f_type_sp ret_type) {
    source_code_t res;
    
    res.push_back({"[&]() {", 1});
    
    res.splice(res.end(), translate(sc.terms, ret_type != f_type::create("unit")));
    res.back().second--;
    
    res.push_back({"} ()", 0});
    return res;
}
source_code_t translate(real_term &t, f_type_sp res_t, var_holder &holder) {
    if ((t.t_1 == nullptr) && (t.t_2 == nullptr)) {
        if (res_t == f_type::create("string")) {
            return {{"\"" + t.val + "\"", 0}};
        }
        return {{t.val, 0}};
    }
    if (t.t_1 == nullptr) {
        source_code_t res({{"(" + t.val, 0}});
        add_code(res, translate(t.t_2, holder));
        add_code(res, ")");
        return res;
    }
    assert(t.t_2 != nullptr);
    
    source_code_t res;
    source_code_t res_1 = translate(t.t_1, holder);
    source_code_t res_2 = translate(t.t_2, holder);
    
    if (t.val == "APP") {
        if (res_t->val == f_type::DEFAULT_VAL) {
            res.push_back({"bind(", 1});
            res.splice(res.end(), res_1);
            res.push_back({",", 0});
            res.splice(res.end(), res_2);
            
            int no = 1;
            f_type_sp c = res_t;
            while (c->val == f_type::DEFAULT_VAL) {
                res.push_back({", placeholders::_" + to_string(no), 0});
                
                no++;
                c = c->t_2;
            }
            res.back().second--;
            res.push_back({")", 0});
        } else {
            res.push_back({"(", 0});
            add_code(res, move(res_1));
            add_code(res, ") (");
            add_code(res, move(res_2));
            add_code(res, ")");
        }
        return res;
    }
    
    res.push_back({"(", 0});
    add_code(res, move(res_1));
    
    add_code(res, t.val);
    
    add_code(res, move(res_2));
    add_code(res, ")");
    return res;
}

source_code_t translate(let_definition_sp let_def, var_holder &holder) {
    source_code_t res;
    
    string fun = to_cpp_type(let_def->full_type);
    
    string first_line = fun + " " + let_def->name + " = ( [&](";
    
    string arguments;
    
    f_type_sp f = let_def->full_type;
    for (pair<string, f_type_sp> &arg : let_def->arguments) {
        if (arguments.size() > 0) {
            arguments += ", ";
        }
        arguments += to_cpp_type(arg.second) + " " + arg.first;
        f = f->t_2;
    }
    
    vector<pair<string, f_type_sp>> new_arguments;
    
    long long no = 1;
    
    while (f->val == f_type::DEFAULT_VAL) {
        string new_name;
        do {
            new_name = "new_arg_" + to_string(no);
            no++;
        } while (visible_vars.count(new_name) > 0);
        
        new_arguments.push_back({new_name, f->t_1});
        
        if (arguments.size() > 0) {
            arguments += ", ";
        }
        arguments += to_cpp_type(f->t_1) + " " + new_name;
        f = f->t_2;
    }
    
    first_line += arguments + ") {";
    
    res.push_back({first_line, 1});
    
    string new_args_to_call;
    if (new_arguments.size() > 0) {
        new_args_to_call = "(";
        for (int w = 0; w < new_arguments.size(); w++) {
            if (w != 0) {
                new_args_to_call += ", ";
            }
            new_args_to_call += new_arguments[w].first;
        }
        new_args_to_call += ")";
    }
    
    res.splice(res.end(), translate(let_def->terms, let_def->ret_type != f_type::create("unit"), new_args_to_call));
    
    res.back().second--;
    res.push_back({"})", 0});
    
    if (arguments.size() == 0) {
        res.back().first.append(" ()");
    }
    return res;
}

source_code_t translate(term_sp t, var_holder &holder) {
    if (holds_alternative<real_term>(t->data)) {
        return translate(get<real_term>(t->data), t->type, holder);
    }
    if (holds_alternative<if_def>(t->data)) {
        return translate(get<if_def>(t->data), holder);
    }
    if (holds_alternative<scope>(t->data)) {
        return translate(get<scope>(t->data), t->type);
    }
    if (holds_alternative<var_name>(t->data)) {
        return translate(get<var_name>(t->data), holder);
    }
}

source_code_t translate(term_seq_sp terms, bool to_return, const string &new_args_to_call) {
    var_holder holder;
    
    source_code_t res;
    for (int e = 0; e < terms->terms.size(); e++) {
        variant<term_sp, let_definition_sp> &w = terms->terms[e];
        source_code_t sub_res;
        if (holds_alternative<term_sp>(w)) {
            sub_res = translate(get<term_sp>(w), holder);
        } else {
            sub_res = translate(get<let_definition_sp>(w), holder);
        }
        
        if ((e == terms->terms.size() - 1) && (new_args_to_call != "")) {
            sub_res.front().first = "(" + move(sub_res.front().first);
            sub_res.back().first += ") " + new_args_to_call;
        }
        sub_res.back().first.append(";");
        
        if ((to_return) && (e == terms->terms.size() - 1)) {
            if (sub_res.size() == 1) {
                res.push_back({"return " + sub_res.back().first, 0});
            } else {
                res.push_back({"return", 1});
                res.splice(res.end(), sub_res);
                res.back().second--;
            }
        } else {
            res.splice(res.end(), sub_res);
        }
    }
    return res;
}

source_code_t generate_working_cpp(term_seq_sp terms) {
    source_code_t res({
        {"#include <iostream>", 0},
        {"#include <functional>", 0},
        {"using namespace std;", 0}
    });
    for (const pair<string, lib_func_info> &w : standart_functions) {
        for (const pair<string, int> &e : w.second.cpp_code) {
            res.push_back(e);
        }
        visible_vars.insert(w.first);
    }
    res.push_back({"int main() {", 1});
    
    res.splice(res.end(), translate(terms, false));
    
    res.push_back({"return 0;", -1});
    res.push_back({"}", 0});
    return res;
}

string cpp_of_ocaml(std::istream* input) {
    term_seq_sp res = parse(input);
    rename_and_inference(res);
    return to_string(generate_working_cpp(res));
}

string to_string(source_code_t source_code) {
    string res;
    int delta = 0;
    for (pair<string, int> &w : source_code) {
        res.append(delta * 2, ' ');
        res += w.first;
        res.append("\n");
        delta += w.second;
    }
    return res;
}
