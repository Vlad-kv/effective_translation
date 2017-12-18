#include <iostream>
#include <fstream>
#include <iomanip>

#include "parser.h"
#include "preparation.h"
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
    return 0;
}
