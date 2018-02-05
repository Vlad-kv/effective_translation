#include "parser.h"
using namespace std;

extern istream *input_stream;
grammar_info_builder input_grammar_result;

int yyparse();

grammar_info parse_input(std::istream& in) {
    input_stream = &in;
    if (yyparse()) {
        throw std::runtime_error("Parser failed.");
    }
    return move(input_grammar_result.extract());
}
