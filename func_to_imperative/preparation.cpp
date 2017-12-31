#include "preparation.h"

#include <cassert>
#include <map>
#include <set>

const std::string f_type::DEFAULT_VAL = "->";

const map<string, lib_func_info> standart_functions = {
    {"print_string", 
        {   "print_string",
            {   {"void print_string(const string& str) {", 1},
                {"cout << str;", -1},
                {"}", 0}
            },
            f_type::create("string -> unit")
        }
    },
    {"print_int",
        {   "print_int",
            {   {"void print_int(int i) {", 1},
                {"cout << i;", -1},
                {"}", 0}
            },
            f_type::create("int -> unit")
        }
    },
    {"read_int",
        {   "read_int",
            {   {"int read_int() {", 1},
                {"int res;", 0},
                {"cin >> res;", 0},
                {"return res;", -1},
                {"}", 0}
            },
            f_type::create("unit -> int")
        }
    },
    {"string_of_int",
        {   "to_string",
            {},
            f_type::create("int -> string")
        }
    }
};

namespace {
    struct operator_t {
        operator_t(string disp, string res_t, vector<string> op_t)
        : disp(disp), res_t(f_type::create(res_t)) {
            for (string &w : op_t) {
                op_types.insert(f_type::create(w));
            }
        }
        operator_t(string disp, string res_t)
        : disp(disp), res_t(f_type::create(res_t)) {
            op_types.insert(this->res_t);
        }
        
        struct comparator {
            bool operator()(const f_type_sp& type_1, const f_type_sp& type_2) const {
                return (type_1 < type_2);
            }
        };
        
        string disp;
        f_type_sp res_t;
        set<f_type_sp, comparator> op_types;
    };
    const map<string, operator_t> binary_operators = {
        {"+", {"+", "int"}},
        {"-", {"-", "int"}},
        {"*", {"*", "int"}},
        {"/", {"/", "int"}},
        {"%", {"%", "int"}},
        
        {"==", {"==", "bool", {"int", "string", "bool"}}},
        {"!=", {"!=", "bool", {"int", "string", "bool"}}},
        {"<=", {"<=", "bool", {"int", "string", "bool"}}},
        {">=", {">=", "bool", {"int", "string", "bool"}}},
        
        {"&&", {"&&", "bool"}},
        {"||", {"||", "bool"}},
        {"^" , {"+" , "string"}}
    };
    
    const map<string, operator_t> unary_operators = {
        {"not", {"!", "bool"}},
        {"-"  , {"-", "int"}}
    };
    
    struct var_info {
        var_info() {
        }
        
        var_info(f_type_sp t, int nesting_level)
        : t(t), nesting_level(nesting_level) {
        }
        
        f_type_sp t;
        int nesting_level; // вложенность
    };
    
    map<string, var_info> visible_vars;
    
    struct vars_holder {
        map<string, var_info> blocked_vars; // to recover on return
        set<string> used_lokal_vars; // to delete from visible_vars on return
        
        void add_var(string &name, f_type_sp t) {
            int level = 0;
            if (visible_vars.count(name) > 0) {
                level = visible_vars[name].nesting_level + 1;
            }
            if (used_lokal_vars.count(name) == 0) {
                used_lokal_vars.insert(name);
                if (visible_vars.count(name) > 0) {
                    blocked_vars[name] = visible_vars[name];
                }
            }
            visible_vars[name] = {t, level};
        }
        
        ~vars_holder() {
            for (const string &w : used_lokal_vars) {
                visible_vars.erase(w);
            }
            for (const pair<string, var_info> &w : blocked_vars) {
                visible_vars[w.first] = w.second;
            }
        }
    };
    
    void upd_name(string &to_update) {
        assert(visible_vars.count(to_update) > 0);
        int level = visible_vars[to_update].nesting_level;
        
        if (level > 0) {
            to_update += "_" + to_string(level);
        }
    }
    
    void update(term_seq_sp seq);
    void update(term_sp t);
    
    void update(term_sp t) {
        if (holds_alternative<real_term>(t->data)) {
            real_term &r_term = get<real_term>(t->data);
            if (r_term.t_1 != nullptr) {
                update(r_term.t_1);
            }
            if (r_term.t_2 != nullptr) {
                update(r_term.t_2);
            }
            if ((r_term.t_1 == nullptr) && (r_term.t_2 != nullptr)) {
                if (unary_operators.count(r_term.val) > 0)  {
                    const operator_t &op = (*unary_operators.find(r_term.val)).second;
                    if (op.op_types.count(r_term.t_2->type) == 0) {
                        throw runtime_error("Incompatible types for unary operator " + op.disp +
                                            " : " + to_string(r_term.t_2->type));
                    }
                    r_term.val = op.disp;
                    
                    t->type = op.res_t;
                    return;
                } else {
                    throw runtime_error("fatal error in update(term_sp t)");
                }
            }
            if ((r_term.t_1 == nullptr) && (r_term.t_2 == nullptr)) {
                return;
            }
            if (r_term.val == "APP") {
                f_type_sp l_t = r_term.t_1->type;
                if ((l_t->val != f_type::DEFAULT_VAL) || (l_t->t_1 != r_term.t_2->type)) {
                    throw runtime_error("Incompatible types for application : " +
                                        to_string(l_t) + "  and  " + to_string(r_term.t_2->type));
                }
                t->type = l_t->t_2;
                return;
            }
            if (binary_operators.count(r_term.val) > 0) {
                const operator_t &op = (*binary_operators.find(r_term.val)).second;
                r_term.val = op.disp;
                
                if ((r_term.t_1->type != r_term.t_2->type) || (op.op_types.count(r_term.t_1->type) == 0)) {
                    throw runtime_error("Incompatible types for binary operator " + op.disp +
                                        " : " + to_string(r_term.t_1->type) + "  and  " +
                                        to_string(r_term.t_2->type));
                }
                t->type = op.res_t;
                return;
            } else {
                throw runtime_error("fatal error : unknown operator " + r_term.val);
            }
        }
        if (holds_alternative<if_def>(t->data)) {
            if_def &if_d = get<if_def>(t->data);
            update(if_d.condition);
            update(if_d.if_branch);
            update(if_d.else_branch);
            
            t->type = if_d.else_branch->type;
        }
        if (holds_alternative<scope>(t->data)) {
            term_seq_sp seq = get<scope>(t->data).terms;
            update(seq);
            
            if (holds_alternative<term_sp>(seq->terms.back())) {
                t->type = get<term_sp>(seq->terms.back())->type;
            } else {
                t->type = f_type::create("unit");
            }
        }
        if (holds_alternative<var_name>(t->data)) {
            var_name &var = get<var_name>(t->data);
            
            if (visible_vars.count(var.name) == 0) {
                throw runtime_error("unbound value : " + var.name);
            }
            t->type = visible_vars[var.name].t;
            upd_name(var.name);
        }
    }
    
    void update(term_seq_sp seq) {
        vars_holder h_1;
        
        for (variant<term_sp, let_definition_sp> &w : seq->terms) {
            if (holds_alternative<term_sp>(w)) {
                update(get<term_sp>(w));
            } else {
                let_definition_sp let_def = get<let_definition_sp>(w);
                string &name = let_def->name;
                
                if (let_def->is_rec) {
                    h_1.add_var(name, let_def->full_type);
                }
                {
                    vars_holder h_2;
                    for (pair<string, f_type_sp> &var : let_def->arguments) {
                        h_2.add_var(var.first, var.second);
                        upd_name(var.first);
                    }
                    update(let_def->terms);
                }
                if (!let_def->is_rec) {
                    h_1.add_var(name, let_def->full_type);
                }
                upd_name(name);
            }
        }
    }
}

void rename_and_inference(term_seq_sp seq) {
    visible_vars.clear();
    for (const pair<string, lib_func_info> &w : standart_functions) {
        visible_vars.insert({w.first, {w.second.type, 0}});
    }
    update(seq);
}
