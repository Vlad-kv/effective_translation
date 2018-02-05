#include <experimental/filesystem>
#include <iostream>
#include <fstream>

#include "tests.h"
#include "../input_reader/parser.h"
using namespace std;

const char* INPUT_GRAMMAR_T = "tests/input_grammar_test_";

namespace fs = experimental::filesystem;

grammar_info single_grammar_test(std::string filename) {
    ifstream in(filename);
    return parse_input(in);
}

void test_input_grammar() {
    for (int w = 1; ; w++) {
        string filename = INPUT_GRAMMAR_T + to_string(w) + ".txt";
        if (fs::exists(filename)) {
            single_grammar_test(filename); 
        } else {
            break;
        }
    }
}
