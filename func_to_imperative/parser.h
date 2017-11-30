#ifndef PARSER_H
#define PARSER_H

#include <iostream>

extern std::istream *input_stream;
extern std::ostream *output_stream;

void parse(std::istream* input, std::ostream *output);

#endif // PARSER_H
