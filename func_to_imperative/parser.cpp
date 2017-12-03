#include "parser.h"
int yyparse();

source_code_t parse(std::istream *input, std::ostream *output) {
    input_stream = input;
    output_stream = output;
    
    if (yyparse()) {
        throw std::runtime_error("Parser failed.");
    }
    source_code_t res(move(*global_result));
    delete global_result;
    return res;
}
