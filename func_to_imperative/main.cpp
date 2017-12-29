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
    
    int a = [&]() {
      return
        (1 + 2);
    } ();
    
    [&]() {
      return
        2,
        3,
        5;
    } ();
    
//    int a = 
//        function<int ()>([]() {
//            cout << "1\n";
//            return 2;
//        })
//     ();;
//    
//    cout << a << "\n";
//    
//    return 0;
    
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
    
    output << to_string(convert(res));
    
    return 0;
}
