#include <iostream>
#include "parser.h"

using namespace std;
int yyparse();

std::vector<std::string> parser_errors;

term_seq_sp parse(std::istream *input, std::ostream *output) {
    input_stream = input;
    output_stream = output;
    
    if (yyparse()) {
        throw std::runtime_error("Parser failed.");
    }
    if (parser_errors.size() > 0) {
        global_result = nullptr;
        
        cout << "Errors:\n";
        for (auto &s : parser_errors) {
            cout << s << "\n";
        }
        throw std::runtime_error("Parser failed.");
    }
    return global_result;
}
