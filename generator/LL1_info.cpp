#include "LL1_info.h"
#include <cassert>
#include <functional>
using namespace std;

std::function<bool (const std::string& s)> LL1_info::default_not_terminal =
    [] (const string& s) {
        return ((s.size() > 0) && ('A' <= s[0]) && (s[0] <= 'Z'));
    };

LL1_info::rule::rule(const string& not_term, const vector<string>& res)
: not_term(not_term), res(res) {
	assert(res.size() > 0);
	if (res.size() > 1) {
		for (auto &s : res) {
			assert(s != "");
		}
	}
}

LL1_info::LL1_info(const std::string& start_not_term, const std::vector<rule>& graph,
                   const std::string& last_sumbol, std::function<bool (const std::string& s)> is_not_terminal)
: g(graph), start_not_term(start_not_term), last_sumbol(last_sumbol), is_not_terminal(is_not_terminal) {
    
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
			throw runtime_error("not_terminal \"" + w + "\" has no rules");
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
void LL1_info::print_map(const std::set<std::string>& s, std::map<std::string, std::set<std::string>>& m, std::ostream& out) {
	for (auto &w : s) {
		out << w << " : ";
		for (const string& str : m[w]) {
			out << "\"" << str << "\"  ";
		}
		out << "\n";
	}
}
void LL1_info::print_first(std::ostream& out) {
	print_map(not_terminals, first, out);
}
void LL1_info::print_follow(std::ostream& out) {
	print_map(not_terminals, follow, out);
}
void LL1_info::print_transitions(std::ostream& out) {
	for (auto &w : rules) {
		out << w.first << ":\n";
		for (const transition &trans : w.second) {
			out << "    ";
			for (const string& str : trans.new_expr) {
				out << "\"" << str << "\" ";
			}
			out << "  :  ";
			for (const string& str : trans.terminals_to_identify) {
				out << "\"" << str << "\" ";
			}
			out << "\n";
		}
	}
}
std::pair<bool, std::string> LL1_info::is_ll1() {
    for (const pair<string, set<transition>>& w : rules) {
        set<string> existing_terminals;
        for (const transition& trans : w.second) {
            for (const string& term : trans.terminals_to_identify) {
                if (existing_terminals.count(term) > 0) {
                    return {false, "ambiguity on terminal \"" + term +
                                "\" in not-terminal \"" + w.first + "\""};
                }
                existing_terminals.insert(term);
            }
        }
    }
    return {true, ""};
}
LL1_info::transition::transition() {
}
LL1_info::transition::transition(const std::vector<std::string>& new_expr, const std::set<std::string>& terminals_to_identify)
: new_expr(new_expr), terminals_to_identify(terminals_to_identify){
}
bool operator<(const LL1_info::transition& t_1, const LL1_info::transition& t_2) {
	return (t_1.new_expr < t_2.new_expr);
}
void LL1_info::upd_set(std::set<std::string>& dest, const std::string& str) {
	if (dest.count(str) == 0) {
		dest.insert(str);
		is_update_happened = true;
	}
}
void LL1_info::upd_set(std::set<std::string>& dest, const std::set<std::string>& sourse) {
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

std::set<std::string> LL1_info::calc_first(const std::vector<std::string>& v, size_t start_pos) {
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
