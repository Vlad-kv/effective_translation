#include "translation.h"
#include <map>
#include <string>
#include "parser.h"
using namespace std;
term_seq_sp global_result = nullptr;

const map<string, pair<string, f_type_sp>> operators = {
    {"+", {"+", f_type::create("int")}},
    {"-", {"-", f_type::create("int")}},
    {"*", {"*", f_type::create("int")}},
    {"/", {"/", f_type::create("int")}},
    
    {"AND", {"&&", f_type::create("bool")}},
    {"OR" , {"||", f_type::create("bool")}},
    {"^", {"+", f_type::create("string")}}
};

code_and_type_t *concatenate_terms_with_operator(code_and_type_t* code_1, std::string op, 
                                                 code_and_type_t* code_2) {
    if (((*operators.find(op)).second.second != code_1->second) || (code_1->second != code_2->second)) {
        parser_errors.push_back("Incompatible types for operator " + op + " : " +
                                to_string(code_1->second) + " " + to_string(code_2->second));
        delete code_2;
        return code_1;
    }
    f_type_sp t = code_1->second;
    
    if ((code_1->first.size() == 1) && (code_2->first.size() == 1)) {
        string res_str = "(" + code_1->first.front().first + " " + op + " " + 
                               code_2->first.front().first + ")";
        delete code_1;
        delete code_2;
        return new code_and_type_t({{{move(res_str), 0}}, t});
    } else {
        source_code_t res;
        res.push_back({"(", 1});
        res.splice(res.end(), move(code_1->first));
        res.push_back({op, 0});
        res.splice(res.end(), move(code_2->first));
        res.back().second--;
        res.push_back({")", 0});
        
        delete code_1;
        delete code_2;
        
        return new code_and_type_t(move(res), t);
    }
}

const map<string, pair<string, f_type_sp>> single_operators = {
    {"not", {"!", f_type::create("bool")}},
    {"-", {"-", f_type::create("int")}}
};

code_and_type_t *concatenate_term_with_unary_operator(std::string op, code_and_type_t* code_1) {
    if ((*single_operators.find(op)).second.second != code_1->second) {
        parser_errors.push_back("Incompatible types for operator " + op + " : " +
                                to_string(code_1->second));
        return code_1;
    }
    f_type_sp t = code_1->second;
    
    if (code_1->first.size() == 1) {
        string res_str = "(" + op + " " + code_1->first.front().first + ")";
        delete code_1;
        
        return new code_and_type_t({{{move(res_str), 0}}, t});
    } else {
        source_code_t res;
        res.push_back({"(", 1});
        res.push_back({op, 0});
        res.splice(res.end(), move(code_1->first));
        res.back().second--;
        res.push_back({")", 0});
        delete code_1;
        
        return new code_and_type_t(res, t);
    }
}

