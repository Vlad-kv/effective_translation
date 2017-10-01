#ifndef WORK_WITH_LL1
#define WORK_WITH_LL1

#include <iostream>
#include <vector>
#include <set>
#include <map>

using namespace std;
// Нетерминал начинается с большой латинской буквы, "эпсилон" - пустая строка.

bool is_not_terminal(const string& s);

struct rule {
	rule(const string& not_term, const vector<string>& res);
	
	string not_term;
	vector<string> res;
};

struct grammatic_info {
	grammatic_info(const string& start_not_term, const vector<rule>& graph, const string& last_sumbol = "$");
	
	static void print_map(const set<string>& s, map<string, set<string>>& m);
	void print_first();
	void print_follow();
	void print_transitions();
	
	vector<rule> g;
	string start_not_term, last_sumbol;
	
	struct transition {
		transition();
		transition(const vector<string>& new_expr, const set<string>& terminals_to_identify);
		
		friend bool operator<(const transition& t_1, const transition& t_2);
		
		vector<string> new_expr;
		mutable set<string> terminals_to_identify;
	};
	
	set<string> not_terminals;
	map<string, set<transition>> rules;
	
	map<string, set<string>> first;
	map<string, set<string>> follow;
private:
	bool is_update_happened;
	
	void upd_set(set<string>& dest, const string& str);
	void upd_set(set<string>& dest, const set<string>& sourse);
	
	set<string> calc_first(const vector<string>& v, size_t start_pos = 0);
};

#endif // WORK_WITH_LL1
