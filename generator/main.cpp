#include <iostream>
#include <fstream>
#include "tests/tests.h"

using namespace std;
int main() {
    try {
        test_input_grammar();
    } catch (runtime_error err) {
        cout << "Error : " << err.what() << "\n";
        return 0;
    }
    {
        ofstream out("output.txt");
        out << single_grammar_test("tests/input_grammar_test_1.txt");
    }
    return 0;
}
