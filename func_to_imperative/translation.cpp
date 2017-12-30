#include <map>
#include <set>
#include <string>
#include <cassert>

#include "translation.h"
#include "parser.h"

using namespace std;
term_seq_sp global_result = nullptr;

namespace {
    set<string> visiable_vars({"print_string", "string_of_int"});
    
    struct var_holder {
        var_holder() {
        }
        void insert(string var) {
            vars.push_back(var);
            visiable_vars.insert(var);
        }
        var_holder& operator = (const var_holder& h) = delete; 
        ~var_holder() {
            for (string &w : vars) {
                visiable_vars.erase(w);
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
        t = t->t_1;
    }
    return "function<" + t->val + " (" + arguments + ")>";
}

source_code_t translate(term_seq_sp terms, bool to_return);
source_code_t translate(term_sp t, var_holder &holder);

source_code_t translate(if_def &if_d, var_holder &holder) {
    // TODO
    throw runtime_error("Not implemented.");
}

source_code_t translate(var_name &var_n, var_holder &holder) {
    holder.insert(var_n.name);
    return {{var_n.name, 0}};
}

source_code_t translate(scope &sc, f_type_sp ret_type) {
    source_code_t res;
    
    res.push_back({"[&]() {", 1});
    
//    if (ret_type != f_type::create("unit")) {
//        res.push_back({"return", 1});
//    }
    res.splice(res.end(), translate(sc.terms, ret_type != f_type::create("unit")));
//    res.back().first.append(";");
    res.back().second--;
    
//    if (ret_type != f_type::create("unit")) {
//        res.back().second--;
//    }
    res.push_back({"} ()", 0});
    return res;
}
source_code_t translate(real_term &t, var_holder &holder) {
    if ((t.t_1 == nullptr) && (t.t_2 == nullptr)) {
        return {{t.val, 0}};
    }
    if (t.t_1 == nullptr) {
        source_code_t sub_res = translate(t.t_2, holder);
        if (sub_res.size() == 1) {
            return {{t.val + sub_res.front().first, 0}};
        }
        source_code_t res({{t.val, 0}});
        res.splice(res.end(), sub_res);
        return res;
    }
    assert(t.t_2 != nullptr);
    
    source_code_t res;
    source_code_t res_1 = translate(t.t_1, holder);
    source_code_t res_2 = translate(t.t_2, holder);
    
    if ((res_1.size() == 1) && (res_2.size() == 1)) {
        return {{"(" + res_1.front().first + " " + t.val + " " + res_2.front().first + ")", 0}};
    }
    
    res.push_back({"(", 1});
    
    res.splice(res.end(), res_1);
    res.back().second--;
    
    res.push_back({t.val, 1});
    
    res.splice(res.end(), res_2);
    res.back().second--;
    
    res.push_back({")", 0});
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
        } while (visiable_vars.count(new_name) > 0);
        
        new_arguments.push_back({new_name, f->t_1});
        
        if (arguments.size() > 0) {
            arguments += ", ";
        }
        arguments += to_cpp_type(f->t_1) + " " + new_name;
        f = f->t_2;
    }
    
    first_line += arguments + ") {";
    
    if (new_arguments.size() > 0) {
        // TODO
        throw runtime_error("Not implemented.");
    }
    
    res.push_back({first_line, 1});
    
    res.splice(res.end(), translate(let_def->terms, let_def->ret_type != f_type::create("unit")));
    
    res.back().second--;
    res.push_back({"})", 0});
    
    if (arguments.size() == 0) {
        res.back().first.append(" ()");
    }
    return res;
}

source_code_t translate(term_sp t, var_holder &holder) {
    if (holds_alternative<real_term>(t->data)) {
        return translate(get<real_term>(t->data), holder);
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

source_code_t translate(term_seq_sp terms, bool to_return) {
    var_holder holder;
    
    source_code_t res;
    for (int e = 0; e < terms->terms.size(); e++) {
        if ((to_return) && (e == terms->terms.size() - 1)) {
            res.push_back({"return", 1});
        }
        variant<term_sp, let_definition_sp> &w = terms->terms[e];
        if (holds_alternative<term_sp>(w)) {
            res.splice(res.end(), translate(get<term_sp>(w), holder));
        } else {
            res.splice(res.end(), translate(get<let_definition_sp>(w), holder));
        }
        res.back().first.append(";");
        
        if ((to_return) && (e == terms->terms.size() - 1)) {
            res.back().second--;
        }
    }
    return res;
}

source_code_t convert(term_seq_sp terms) {
    return translate(terms, false);
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
