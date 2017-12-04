#include "parser.h"
#include <iostream>
int yyparse();

using namespace std;

std::vector<std::string> parser_errors;

source_code_t parse(std::istream *input, std::ostream *output) {
    input_stream = input;
    output_stream = output;
    
    if (yyparse()) {
        throw std::runtime_error("Parser failed.");
    }
    if (parser_errors.size() > 0) {
        delete global_result;
        
        cout << "Errors:\n";
        for (auto &s : parser_errors) {
            cout << s << "\n";
        }
        throw std::runtime_error("Parser failed.");
    }
    source_code_t res(move(*global_result));
    delete global_result;
    return res;
}
