#include <experimental/filesystem>
#include <iostream>
#include <fstream>

#include "tests.h"
#include "translation.h"
using namespace std;

namespace fs = experimental::filesystem;

const string TEST_DIR = "tests/";

int test(string name) {
    int res = system(("ocamlc -o " + TEST_DIR + name + "_ocaml.exe " + TEST_DIR + name + ".ml").data());
    if (res != 0) {
        cout << "incorrect source code in " + name << "\n";
        return 1;
    }
    {
        ifstream in(TEST_DIR + name + ".ml");
        ofstream out(TEST_DIR + name + ".cpp");
        
        try {
            out << cpp_of_ocaml(&in);
        } catch (runtime_error err) {
            cout << "Error (in " + name + ") : " << err.what() << "\n";
            return 1;
        }
    }
    res = system(("g++ -std=c++17 -o " + TEST_DIR + name + "_cpp.exe " + TEST_DIR + name + ".cpp").data());
    if (res != 0) {
        cout << "incorrect generation in " + name + "\n";
        return 1;
    }
    return 0;
}

void test_all_correct() {
    int no = 1;
    while (true) {
        string name = "test_correct_";
        if (no < 10) {
            name += "0";
        }
        name += to_string(no);
        
        if (fs::exists(TEST_DIR + name + ".ml")) {
            if (test(name)) {
                return;
            }
        } else {
            break;
        }
        no++;
    }
}
