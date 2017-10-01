#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>

#include "lexical_analyzer.h"
#include "work_with_LL1.h"
using namespace std;
using namespace tokens_constants;

int main() {
	ifstream z("input.txt");
//	ofstream x("output.txt");
	int q, w, e, r;
    
//    lexical_analyzer analyzer(z);
//    
//    token prev_t = CHAR;
//    while (prev_t != END) {
//		try {
//			prev_t = analyzer.get_next_token();
//			if (prev_t == L_BRACE) {
//				cout << "L_BRACE\n";
//			}
//		} catch (runtime_error err) {
//			cout << err.what() << "\n";
//			return 0;
//		}
//    }
    
    try {
    	grammatic_info info(
			"R",
			{
				{"R", {"O", "R'"}},
				{"R'", {""}},
				{"R'", {"|", "R"}},
				{"O", {"C", "O"}},
				{"O", {""}},
				{"C'", {"*", "C'"}},
				{"C'", {""}},
				{"C", {"c", "C'"}},
				{"C", {"(", "R", ")", "C'"}}
			}
		);
		info.print_first();
		cout << "-------------\n";
		info.print_follow();
		cout << "-------------\n";
		info.print_transitions();
		
    } catch (runtime_error err) {
    	cout << "runtime_error : " << err.what() << "\n";
    	return 0;
    }
    
    
    return 0;
}
