#include "parser.h"

int yyparse();

void parse(std::istream *input, std::ostream *output) {
    input_stream = input;
    output_stream = output;
    
    if (yyparse()) {
        throw std::runtime_error("Parser failed.");
    }
}
