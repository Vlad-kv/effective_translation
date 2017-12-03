#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <list>
#include <iostream>

typedef std::list<std::pair<std::string, int>> source_code_t;

extern source_code_t *global_result;

source_code_t *concatenate_terms_with_operator(source_code_t* code_1, std::string op, 
                                             source_code_t* code_2);
source_code_t *concatenate_term_with_unary_operator(std::string op, source_code_t* code_1);

#endif // TRANSLATION_H
