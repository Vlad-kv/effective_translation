#include <iostream>
#include <fstream>
#include <iomanip>

#include <functional>

#include "parser.h"
#include "preparation.h"
#include "translation.h"
using namespace std;

int main () {
    ifstream input("input.txt");
    ofstream output("output.txt");
    
    term_seq_sp res;
    
    try {
        res = parse(&input, &output);
        output << to_string(res) << "\n";
    } catch (runtime_error error) {
        cout << error.what() << "\n";
        return 0;
    }
    output << "\n\n\n##########\n##########\n\n";
    
    try {
        rename_and_inference(res);
        output << to_string(res) << "\n";
    } catch (runtime_error error) {
        cout << error.what() << "\n";
        return 0;
    }
    
    output << "\n\n**********\n**********\n**********\n**********\n\n";
    
    output << to_string(generate_working_cpp(res));
    
    return 0;
}
