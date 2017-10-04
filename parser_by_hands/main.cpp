#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>

#include "lexical_analyzer.h"
#include "parser.h"
#include "work_with_LL1.h"
#include "visualization.h"
using namespace std;
using namespace tokens_constants;

void print_grammatic_info() {
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
    	return;
    }
}

int main() {
	ifstream z("input.txt");
	freopen("output.txt", "w", stdout);
	int q, w, e, r;
    
//    print_grammatic_info();
//    return 0;
    
    lexical_analyzer analyzer(z);
    parser my_p;
    expr_sp res;
    try {
		res = my_p.parse(analyzer);
    } catch (runtime_error err) {
    	cout << "runtime_error : " << err.what() << "\n";
    	return 0;
    }
    cout << to_string(res) << "\n\n";
    visualize(res);
    return 0;
}
