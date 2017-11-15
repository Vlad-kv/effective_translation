#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include "lexical_analyzer.h"
using namespace std;

class expr;
typedef shared_ptr<expr> expr_sp;

class expr {
public:
	expr(string not_terminal_name, const token& t = tokens_constants::END);
	
	void add_to_childrens(expr_sp ex);
	void reverse();
	
	string not_terminal_name;
	token t;
	expr_sp next = nullptr, child = nullptr;
};

string to_string(expr_sp ex);

class parser {
public:
	expr_sp parse(lexical_analyzer& analyzer);
	token read_next();
private:
	void check(bool assertion, string mess);
	void check_token(token t);
	void unexpected_token_error();
	
	template<typename ...Args>
	bool compare_to_curr_token(token first, Args ...other) {
		if (first == curr_token) {
			return true;
		}
		return compare_to_curr_token(other...);
	}
	bool compare_to_curr_token() {
		return false;
	}
	
	expr_sp C();
	expr_sp C_();
	expr_sp O();
	expr_sp O_();
	expr_sp R();
	expr_sp R_();
	expr_sp M();
	
	token curr_token;
	size_t no_of_token;
	lexical_analyzer* analyzer = nullptr;
};

#endif // PARSER_H
