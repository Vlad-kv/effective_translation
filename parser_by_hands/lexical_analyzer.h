#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#include <iostream>
using namespace std;

class token {
public:
	token(char val = 0);
	token(const token& t);
	
	char get_val() const;
	
	friend bool operator==(const token& t_1, const token& t_2);
	friend bool operator!=(const token& t_1, const token& t_2);
	
	friend bool operator<(const token& t_1, const token& t_2);
private:
	char val;
};

namespace tokens_constants {
	extern const token ASTERISK;
	extern const token OR;
	extern const token END;
	extern const token L_BRACE;
	extern const token R_BRACE;
	extern const token CHAR;
}
class lexical_analyzer {
public:
	lexical_analyzer(std::istream& input);
	
	token get_next_token();
private:
	static bool is_space(char c);
	
	std::istream& input;
	int pos = 0;
};

#endif // LEXICAL_ANALYZER_H
