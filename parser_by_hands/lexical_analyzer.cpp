#include "lexical_analyzer.h"

using namespace std;
using namespace tokens_constants;

namespace tokens_constants {
	const token ASTERISK('*');
	const token OR('|');
	const token END('$');
	const token L_BRACE('(');
	const token R_BRACE(')');
	const token CHAR('a');
}

token::token(char val) 
: val(val) {
}
token::token(const token& t)
: val(t.val) {
}
char token::get_val() const {
	return val;
}
bool operator==(const token& t_1, const token& t_2) {
	if (('a' <= min(t_1.val, t_2.val)) && (max(t_1.val, t_2.val) <= 'z')) {
		return true;
	}
	return (t_1.val == t_2.val);
}
bool operator!=(const token& t_1, const token& t_2) {
	return !(t_1 == t_2);
}
bool operator<(const token& t_1, const token& t_2) {
	if (t_1 == t_2) {
		return false;
	}
	return (t_1.val < t_2.val);
}

lexical_analyzer::lexical_analyzer(std::istream& input)
: input(input) {
}

token lexical_analyzer::get_next_token() {
	using namespace tokens_constants;
	char res;
	do {
		res = input.get();
		pos++;
	} while (is_space(res));
	if (res == char_traits<char>::eof()) {
		return {'$'};
	}
	token result(res);
	if ((result != ASTERISK) && (result != OR) && (result != END) && (result != L_BRACE) &&
		(result != R_BRACE) && (result != CHAR)) {
		throw runtime_error("Forbidden char with code " + to_string((int)res) + " on position " + to_string(pos));
	}
	return result;
}

bool lexical_analyzer::is_space(char c) {
	return ((c == ' ') || (c == '\n') || (c == '\r') || (c == '\t'));
}
