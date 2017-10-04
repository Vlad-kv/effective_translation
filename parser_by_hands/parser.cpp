#include "parser.h"

#include <set>
using namespace std;
using namespace tokens_constants;

expr::expr(string not_terminal_name, const token& t)
: not_terminal_name(not_terminal_name), t(t) {
}
void expr::add_to_childrens(expr_sp ex) {
	ex->next = child;
	child = ex;
}
void expr::reverse() {
	expr_sp c = child;
	child = nullptr;
	for (; c != nullptr;) {
		c->reverse();
		expr_sp v = c->next;
		add_to_childrens(c);
		c = v;
	}
}
string to_string(expr_sp ex) {
	string res = ex->not_terminal_name;
	if (res == "") {
		res.push_back(ex->t.get_val());
		return res;
	}
	res += "(";
	for (expr_sp c = ex->child; c != nullptr; c = c->next) {
		res += to_string(c);
	}
	return res + ")";
}
expr_sp parser::parse(lexical_analyzer& analyzer) {
	this->analyzer = &analyzer;
	no_of_token = 0;
	read_next();
	
	expr_sp res = R();
	check_token(END);
	
	res->reverse();
	return res;
}
token parser::read_next() {
	curr_token = analyzer->get_next_token();
	return curr_token;
}
void parser::check(bool assertion, string mess) {
	if (assertion == false) {
		throw runtime_error(mess);
	}
}
void parser::check_token(token t) {
	if (curr_token != t) {
		unexpected_token_error();
	}
}
void parser::unexpected_token_error() {
	string mess = "unexpected token \"";
	mess.push_back(curr_token.get_val());
	throw runtime_error(mess + "\" on with number " + to_string(no_of_token));
}
expr_sp parser::C() {
	expr_sp res = make_shared<expr>("C");
	if (curr_token == L_BRACE) {
		read_next();
		res->add_to_childrens(R());
		check_token(R_BRACE);
		read_next();
		res->add_to_childrens(C_());
		return res;
	}
	if (curr_token == CHAR) {
		res->add_to_childrens(make_shared<expr>("", curr_token.get_val()));
		read_next();
		res->add_to_childrens(C_());
		return res;
	}
	unexpected_token_error();
	return nullptr;
}
expr_sp parser::C_() {
	expr_sp res = make_shared<expr>("C_");
	if (compare_to_curr_token(END, L_BRACE, R_BRACE, CHAR, OR)) {
		return res;
	}
	if (curr_token == ASTERISK) {
		read_next();
		res->add_to_childrens(C_());
		return res;
	}
	unexpected_token_error();
	return nullptr;
}
expr_sp parser::O() {
	expr_sp res = make_shared<expr>("O");
	if (compare_to_curr_token(END, R_BRACE, OR)) {
		return res;
	}
	if (compare_to_curr_token(L_BRACE, CHAR)) {
		res->add_to_childrens(C());
		res->add_to_childrens(O());
		return res;
	}
	unexpected_token_error();
	return nullptr;
}
expr_sp parser::R() {
	expr_sp res = make_shared<expr>("R");
	if (compare_to_curr_token(END, L_BRACE, R_BRACE, CHAR, OR)) {
		res->add_to_childrens(O());
		res->add_to_childrens(R_());
		return res;
	}
	unexpected_token_error();
	return nullptr;
}
expr_sp parser::R_() {
	expr_sp res = make_shared<expr>("R_");
	if (compare_to_curr_token(END, R_BRACE)) {
		return res;
	}
	if (curr_token == OR) {
		read_next();
		res->add_to_childrens(R());
		return res;
	}
	unexpected_token_error();
	return nullptr;
}
