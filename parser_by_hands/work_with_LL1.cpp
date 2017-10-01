#include "work_with_LL1.h"
#include <cassert>
#include <functional>
using namespace std;

bool is_not_terminal(const string& s) {
	return ((s.size() > 0) && ('A' <= s[0]) && (s[0] <= 'Z'));
}

rule::rule(const string& not_term, const vector<string>& res)
: not_term(not_term), res(res) {
	assert(is_not_terminal(not_term));
	assert(res.size() > 0);
	if (res.size() > 1) {
		for (auto &s : res) {
			assert(s != "");
		}
	}
}

grammatic_info::grammatic_info(const string& start_not_term, const vector<rule>& graph, const string& last_sumbol)
: g(graph), start_not_term(start_not_term), last_sumbol(last_sumbol) {
	assert(is_not_terminal(start_not_term));
	
	not_terminals.insert(start_not_term);
	
	for (rule &w : g) {
		not_terminals.insert(w.not_term);
		for (string &s : w.res) {
			if (is_not_terminal(s)) {
				not_terminals.insert(s);
			}
		}
		rules[w.not_term].insert({w.res, {}});
	}
	for (auto &w : not_terminals) {
		if (rules.count(w) == 0) {
			throw runtime_error("not_terminal " + w + " has no rules");
		}
	}
	is_update_happened = true;
	while (is_update_happened) {
		is_update_happened = false;
		for (const rule& w : g) {
			upd_set(first[w.not_term], calc_first(w.res));
		}
	}
	
	follow[start_not_term].insert(last_sumbol);
	is_update_happened = true;
	while (is_update_happened) {
		is_update_happened = false;
		for (const rule& w : g) {
			for (size_t e = 0; e < w.res.size(); e++) {
				if (is_not_terminal(w.res[e])) {
					set<string> res = calc_first(w.res, e + 1);
					if (res.count("") > 0) {
						res.erase("");
						upd_set(follow[w.res[e]], follow[w.not_term]);
					}
					upd_set(follow[w.res[e]], res);
				}
			}
		}
	}
	
	for (auto &w : rules) {
		for (const transition &trans : w.second) {
			trans.terminals_to_identify = calc_first(trans.new_expr);
			if (trans.terminals_to_identify.count("") > 0) {
				trans.terminals_to_identify.erase("");
				upd_set(trans.terminals_to_identify, follow[w.first]);
			}
		}
	}
}
void grammatic_info::print_map(const set<string>& s, map<string, set<string>>& m) {
	for (auto &w : s) {
		cout << w << " : ";
		for (const string& str : m[w]) {
			cout << "\"" << str << "\"  ";
		}
		cout << "\n";
	}
}
void grammatic_info::print_first() {
	print_map(not_terminals, first);
}
void grammatic_info::print_follow() {
	print_map(not_terminals, follow);
}
void grammatic_info::print_transitions() {
	for (auto &w : rules) {
		cout << w.first << ":\n";
		for (const transition &trans : w.second) {
			cout << "    ";
			for (const string& str : trans.new_expr) {
				cout << "\"" << str << "\" ";
			}
			cout << "  :  ";
			for (const string& str : trans.terminals_to_identify) {
				cout << "\"" << str << "\" ";
			}
			cout << "\n";
		}
	}
}
grammatic_info::transition::transition() {
}
grammatic_info::transition::transition(const vector<string>& new_expr, const set<string>& terminals_to_identify)
: new_expr(new_expr), terminals_to_identify(terminals_to_identify){
}
bool operator<(const grammatic_info::transition& t_1, const grammatic_info::transition& t_2) {
	return (t_1.new_expr < t_2.new_expr);
}
void grammatic_info::upd_set(set<string>& dest, const string& str) {
	if (dest.count(str) == 0) {
		dest.insert(str);
		is_update_happened = true;
	}
}
void grammatic_info::upd_set(set<string>& dest, const set<string>& sourse) {
	for (auto &w : sourse) {
		upd_set(dest, w);
	}
}

struct on_return {
	on_return(function<void ()> f)
	: f(f) {
	}
	~on_return() {
		f();
	}
	function<void ()> f;
};

set<string> grammatic_info::calc_first(const vector<string>& v, size_t start_pos) {
	set<string> res;
	
	bool old_val = is_update_happened;
	on_return on_ret([old_val, this]() {is_update_happened = old_val;});
	is_update_happened = false;
	
	for (size_t w = start_pos; w < v.size(); w++) {
		const string& str = v[w];
		if (is_not_terminal(str)) {
			set<string>& interest = first[str];
			if (interest.count("") == 0) {
				upd_set(res, interest);
				return res;
			} else {
				interest.erase("");
				upd_set(res, interest);
				interest.insert("");
			}
		} else {
			upd_set(res, str);
			return res;
		}
	}
	upd_set(res, "");
	return res;
}
