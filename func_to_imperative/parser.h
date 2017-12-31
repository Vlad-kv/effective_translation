#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <vector>

extern std::vector<std::string> parser_errors;

#include "translation.h"
#include "functional_grammar.h"

extern std::istream *input_stream;

term_seq_sp parse(std::istream* input);

#endif // PARSER_H
