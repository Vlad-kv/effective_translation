#include <iostream>
#include <set>
#include <map>

#include "input_file_grammar.tab.hpp"
using namespace std;

istream *input_stream;

bool is_space(char c) {
	return ((c == ' ') || (c == '\n') || (c == '\r') || (c == '\t'));
}

const set<char> SPECIAL_SUMBOLS = {
    ':', ';', '|', '{', '}', '<', '>'
};
const map<string, int> KEY_WORDS = {
    {"%%", SEPARATOR},
    {"%l", LEFT},
    {"%r", RIGHT}
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
    if (c == '{') { // CPP_CODE
        string buff;
        int delta = 1;
        bool is_in_str = false;
        char prev;
        
        while (true) {
            prev = c;
            c = input_stream->get();
            if (c == char_traits<char>::eof()) {
                return LEX_ANALYSER_ERROR;
            }
            if ((c == '"') && (prev != '\\')) {
                is_in_str ^= 1;
            }
            if (! is_in_str) {
                if (c == '{') {
                    delta++;
                }
                if (c == '}') {
                    delta--;
                    if (delta == 0) {
                        break;
                    }
                }
            }
            buff.push_back(c);
        }
        yylval.string_ptr_val = new string();
        *yylval.string_ptr_val = move(buff);
        return CPP_CODE;
    }
    if (c == '\'') { // CHAR_TOKEN
        c = input_stream->get();
        if (c == '\\') {
            c = input_stream->get();
        }
        int next = input_stream->get();
        if (next != '\'') {
            return LEX_ANALYSER_ERROR;
        } else {
            yylval.char_val = c;
            return CHAR_TOKEN;
        }
    }
    if (is_normal_sumbol(c)) {
        string buff({(char)c});
        while (true) {
            c = input_stream->peek();
            if (!is_normal_sumbol(c)) {
                break;
            }
            buff.push_back(input_stream->get());
        }
        
        if (KEY_WORDS.count(buff) > 0) {
            return (*KEY_WORDS.find(buff)).second;
        }
        yylval.string_ptr_val = new string();
        *yylval.string_ptr_val = move(buff);
        return WORD;
    }
    if (c == char_traits<char>::eof()) {
        return 0;
    }
    return c;
}
