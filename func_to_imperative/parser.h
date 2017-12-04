#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <vector>
#include "translation.h"

extern std::istream *input_stream;
extern std::ostream *output_stream;

extern std::vector<std::string> parser_errors;

source_code_t parse(std::istream* input, std::ostream *output);

#endif // PARSER_H
