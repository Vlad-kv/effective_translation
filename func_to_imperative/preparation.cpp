#include "preparation.h"

#include <cassert>
#include <map>
#include <set>

const std::string f_type::DEFAULT_VAL = "->";

namespace {
    const map<string, pair<string, f_type_sp>> binary_operators = {
        {"+", {"+", f_type::create("int")}},
        {"-", {"-", f_type::create("int")}},
        {"*", {"*", f_type::create("int")}},
        {"/", {"/", f_type::create("int")}},
        {"%", {"%", f_type::create("int")}},
        
        {"==", {"==", f_type::create("bool")}},
        {"!=", {"!=", f_type::create("bool")}},
        {"<=", {"<=", f_type::create("bool")}},
        {">=", {">=", f_type::create("bool")}},
        
        {"&&", {"&&", f_type::create("bool")}},
        {"||", {"||", f_type::create("bool")}},
        {"^", {"+", f_type::create("string")}}
    };
    
    const map<string, pair<string, f_type_sp>> unary_operators = {
        {"not", {"!", f_type::create("bool")}},
        {"-"  , {"-", f_type::create("int")}}
    };
    const map<string, pair<string, f_type_sp>> standart_functions = {
        {"print_string", {"print_string", f_type::create("string -> unit")}},
        {"string_of_int", {"to_string", f_type::create("int -> string")}}
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
                    const pair<string, f_type_sp> &p = (*unary_operators.find(r_term.val)).second;
                    if (p.second != r_term.t_2->type) {
                        throw runtime_error("Incompatible types for unary operator " + p.first +
                                            " : " + to_string(r_term.t_2->type));
                    }
                    r_term.val = p.first;
                    
                    t->type = p.second;
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
                const pair<string, f_type_sp> &p = (*binary_operators.find(r_term.val)).second;
                r_term.val = p.first;
                
                if ((r_term.t_1->type != r_term.t_2->type) || (r_term.t_1->type != p.second)) {
                    throw runtime_error("Incompatible types for binary operator " + p.first +
                                        " of type " + to_string(p.second) + " : " +
                                        to_string(r_term.t_1->type) + "  and  " +
                                        to_string(r_term.t_2->type));
                }
                t->type = p.second;
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
    for (const pair<string, pair<string, f_type_sp>> &w : standart_functions) {
        visible_vars.insert({w.first, {w.second.second, 0}});
    }
    update(seq);
}
