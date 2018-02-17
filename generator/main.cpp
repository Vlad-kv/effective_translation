#include <iostream>
#include <fstream>
#include "tests/tests.h"
#include "LL1_info.h"
using namespace std;

int main() {
    try {
        test_input_grammar();
    } catch (runtime_error err) {
        cout << "Error : " << err.what() << "\n";
        return 0;
    }
    {
        grammar_info info = single_grammar_test("tests/input_grammar_test_2.txt");
        info.calculate_LL1_info();
        
        LL1_info &ll1 = *info.ll1_info;
        
        ofstream out("output.txt");
        out << info;
        ll1.print_first(out);
        out << "---\n";
        ll1.print_follow(out);
        out << "---\n";
        ll1.print_transitions(out);
        out << "---\n";
        pair<bool, string> res = ll1.is_ll1();
        if (res.first) {
            out << "Yes\n";
        } else {
            out << "No : " << res.second << "\n";
        }
    }
    return 0;
}
