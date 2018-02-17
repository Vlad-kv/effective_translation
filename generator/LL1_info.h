#ifndef LL1_INFO_H
#define LL1_INFO_H

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <functional>

// Нетерминал начинается с большой латинской буквы, "эпсилон" - пустая строка.

struct LL1_info {
    static std::function<bool (const std::string& s)> default_not_terminal;
    
    struct rule {
        rule(const std::string& not_term, const std::vector<std::string>& res);
        
        std::string not_term;
        std::vector<std::string> res;
    };
    
	LL1_info(const std::string& start_not_term, const std::vector<rule>& graph,
             const std::string& last_sumbol = "$", std::function<bool (const std::string& s)> is_not_terminal = default_not_terminal);
	
	static void print_map(const std::set<std::string>& s, std::map<std::string, std::set<std::string>>& m, std::ostream& out);
	void print_first(std::ostream& out);
	void print_follow(std::ostream& out);
	void print_transitions(std::ostream& out);
	
	std::pair<bool, std::string> is_ll1();
	
	std::vector<rule> g;
	std::string start_not_term, last_sumbol;
	
	struct transition {
		transition();
		transition(const std::vector<std::string>& new_expr, const std::set<std::string>& terminals_to_identify);
		
		friend bool operator<(const transition& t_1, const transition& t_2);
		
		std::vector<std::string> new_expr;
		mutable std::set<std::string> terminals_to_identify;
	};
	
	std::function<bool (const std::string& s)> is_not_terminal;
	
	std::set<std::string> not_terminals;
	std::map<std::string, std::set<transition>> rules;
	
	std::map<std::string, std::set<std::string>> first;
	std::map<std::string, std::set<std::string>> follow;
private:
	bool is_update_happened;
	
	void upd_set(std::set<std::string>& dest, const std::string& str);
	void upd_set(std::set<std::string>& dest, const std::set<std::string>& sourse);
	
	std::set<std::string> calc_first(const std::vector<std::string>& v, size_t start_pos = 0);
};

#endif // LL1_INFO_H
