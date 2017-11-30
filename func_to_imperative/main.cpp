#include <iostream>
#include <fstream>
#include <iomanip>

#include "parser.h"
using namespace std;

int main () {
    ifstream input("input.txt");
    ofstream output("output.txt");
    try {
        parse(&input, &output);
    } catch (runtime_error error) {
        cout << error.what() << "\n";
        return 0;
    }
    return 0;
}
