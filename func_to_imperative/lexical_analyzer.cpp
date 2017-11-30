#include <stdio.h>
#include <ctype.h>

#include <string>
#include <iostream>
#include <set>
#include <map>
#include <vector>

#include "functional_parser.tab.hpp"

using namespace std;

istream *input_stream = nullptr;
ostream *output_stream = nullptr;

void fatal_error(string mess) {
    cout << "Fatal error : " << mess << "\n";
    exit(1);
}

bool is_space(char c) {
	return ((c == ' ') || (c == '\n') || (c == '\r') || (c == '\t'));
}

const set<char> SPECIAL_SUMBOLS = {
    '<', '>', '[', ']', '(', ')', '{', '}', '=', '?', ',', '.',
    '|', '&', '*', '/', '-', '+', '%', '^', '\\', '"', '\'', '!',
    '@', '#', '$', ':', ';', '~'
};
const map<string, int> KEY_WORDS = {
    {"let", LET}
};
bool is_special_sumbol(char c) {
    return SPECIAL_SUMBOLS.count(c) > 0;
}
bool is_normal_sumbol(char c) {
    return ((! is_space(c)) && (! is_special_sumbol(c)) && (c != char_traits<char>::eof()));
}

int yylex() {
    int c = input_stream->get();
    
    while (is_space(c)) {
        c = input_stream->get();
    }
    if (isdigit(c)) {
        if (! input_stream->unget()) {
            fatal_error("input_stream->unget() failed (in lexical_analyzer.cpp)");
        }
        (*input_stream) >> yylval.int_val;
        return INT_NUM;
    }
    if (is_normal_sumbol(c)) {
        string buff;
        do {
            buff.push_back(c);
            c = input_stream->get();
        } while (is_normal_sumbol(c));
        
        if (! input_stream->unget()) {
            fatal_error("input_stream->unget() failed (in lexical_analyzer.cpp)");
        }
        if (KEY_WORDS.count(buff) > 0) {
            return (*KEY_WORDS.find(buff)).second;
        }
        
        yylval.string_ptr_val = new string();
        *yylval.string_ptr_val = move(buff);
        return VAR_NAME;
    }
    if (c == char_traits<char>::eof()) {
        return 0;
    }
    return c;
}
