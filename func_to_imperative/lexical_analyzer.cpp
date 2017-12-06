#include <stdio.h>
#include <ctype.h>

#include <string>
#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <limits>
#include <list>

#include "functional_grammar.h"
#include "translation.h"
using namespace std;

#include "functional_parser.tab.hpp"

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
    {"let", LET},
    {"in", IN},
    {"rec", REC},
    {"fun", FUN},
    {"not", NOT},
    {"mod", MOD},
    
    {"if", IF},
    {"then", THEN},
    {"else", ELSE},
    {"begin", BEGIN},
    {"end", END},
    
    {"unit", UNIT},
    {"int", INT},
    {"string", STRING},
    {"bool", BOOL},
    
    {"true", TRUE},
    {"false", FALSE}
};
const map<string, int> DOUBLE_SUMBOLS = {
    {";;", DOUBLE_SEMICOLON},
    {"->", ARROW},
    {"||", OR},
    {"&&", AND},
    {"<>", NOT_EQUAL},
    {"<=", LESS_OR_EQUAL},
    {">=", MORE_OR_EQUAL}
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
    if (c == '\"') { // string
        string buff;
        char prev = 0;
        while (true) {
            prev = c;
            if (c == char_traits<char>::eof()) {
                return UNFINISHED_STRING;
            }
            c = input_stream->get();
            if ((prev != '\\') && (c == '\"')) {
                yylval.string_ptr_val = new string();
                *yylval.string_ptr_val = move(buff);
                return STRING_CONSTANT;
            }
            buff.push_back(c);
        }
    }
    if (isdigit(c)) {
        input_stream->unget();
        
        (*input_stream) >> yylval.int_val;
        return INT_NUM;
    }
    if (is_normal_sumbol(c)) {
        string buff;
        do {
            buff.push_back(c);
            c = input_stream->get();
        } while (is_normal_sumbol(c));
        
        input_stream->unget();
        
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
    char next_char = input_stream->get();
    string str({(char)c, next_char});
    
    if (DOUBLE_SUMBOLS.count(str) > 0) {
        return (*DOUBLE_SUMBOLS.find(str)).second;
    }
    input_stream->unget();
    return c;
}
