#include <iostream>
#include <fstream>
#include <iomanip>

#include <functional>

#include "parser.h"
#include "preparation.h"
#include "translation.h"
#include "tests.h"
using namespace std;

void example() {
    ifstream input("input.txt");
    ofstream output("output.txt");
    
    term_seq_sp res;
    
    try {
        res = parse(&input);
//        output << to_string(res) << "\n";
    } catch (runtime_error error) {
        cout << error.what() << "\n";
        return;
    }
//    output << "\n\n\n##########\n##########\n\n";
    
    try {
        rename_and_inference(res);
//        output << to_string(res) << "\n";
    } catch (runtime_error error) {
        cout << error.what() << "\n";
        return;
    }
    
//    output << "\n\n**********\n**********\n**********\n**********\n\n";
    
    output << to_string(generate_working_cpp(res));
}

int main () {
//    test_all_correct();
    example();
    return 0;
}
