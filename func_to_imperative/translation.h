#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <list>
#include <iostream>

#include "functional_grammar.h"

typedef std::list<std::pair<std::string, int>> source_code_t;

extern term_seq_sp global_result;

string to_string(source_code_t source_code);

source_code_t generate_working_cpp(term_seq_sp terms);

#endif // TRANSLATION_H
