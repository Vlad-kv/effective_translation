#ifndef PREPARATION_H
#define PREPARATION_H

#include <map>

#include "functional_grammar.h"
#include "translation.h"

void rename_and_inference(term_seq_sp seq);

struct lib_func_info {
    source_code_t cpp_code;
    f_type_sp type;
    
    lib_func_info(source_code_t &&cpp_code, f_type_sp type)
    : cpp_code(move(cpp_code)),
      type(move(type))
     {
    }
};

extern const std::map<string, lib_func_info> standart_functions;

#endif // PREPARATION_H
