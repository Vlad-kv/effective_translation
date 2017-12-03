#include "translation.h"
using namespace std;
source_code_t *global_result = nullptr;

source_code_t *concatenate_terms_with_operator(source_code_t* code_1, std::string op, 
                                             source_code_t* code_2) {
    if ((code_1->size() == 1) && (code_2->size() == 1)) {
        string res_str = "(" + code_1->front().first + " " + op + " " + 
                               code_2->front().first + ")";
        delete code_1;
        delete code_2;
        return new source_code_t({{move(res_str), 0}});
    } else {
        source_code_t *res = new source_code_t();
        res->push_back({"(", 1});
        res->splice(res->end(), move(*code_1));
        res->push_back({op, 0});
        res->splice(res->end(), move(*code_2));
        res->back().second--;
        res->push_back({")", 0});
        
        delete code_1;
        delete code_2;
        
        return res;
    }
}

source_code_t *concatenate_term_with_unary_operator(std::string op, source_code_t* code_1) {
    if (code_1->size() == 1) {
        string res_str = "(" + op + " " + code_1->front().first + ")";
        delete code_1;
        
        return new source_code_t({{move(res_str), 0}});
    } else {
        source_code_t *res = new source_code_t();
        res->push_back({"(", 1});
        res->push_back({op, 0});
        res->splice(res->end(), move(*code_1));
        res->back().second--;
        res->push_back({")", 0});
        delete code_1;
        
        return res;
    }
}

