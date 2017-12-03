#include <iostream>
#include <fstream>
#include <iomanip>

#include "parser.h"
using namespace std;

int main () {
    ifstream input("input.txt");
    ofstream output("output.txt");
    
    try {
        source_code_t res = parse(&input, &output);
        int delta = 0;
        for (pair<string, int> &w : res) {
            for (int e = 0; e < delta; e++) {
                output << "    ";
            }
            output << w.first << "\n";
            delta += w.second;
        }
    } catch (runtime_error error) {
        cout << error.what() << "\n";
        return 0;
    }
    return 0;
}
