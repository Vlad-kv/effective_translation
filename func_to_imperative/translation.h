#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <list>
#include <iostream>

#include "functional_grammar.h"

typedef std::list<std::pair<std::string, int>> source_code_t;
typedef std::pair<source_code_t, f_type_sp> code_and_type_t;

extern term_seq_sp global_result;

code_and_type_t *concatenate_terms_with_operator(code_and_type_t* code_1, std::string op, 
                                                 code_and_type_t* code_2);
source_code_t *concatenate_term_with_unary_operator(std::string op, source_code_t* code_1);

#endif // TRANSLATION_H
