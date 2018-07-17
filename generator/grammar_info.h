#ifndef GRAMMAR_INFO_H
#define GRAMMAR_INFO_H

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <memory>

#include "LL1_info.h"

class grammar_info {
public:
    struct token {
        token(int val, const std::string& type);
        
        int val;
        std::string type;
    };
    enum associativity{left, right};
    struct operator_info {
        operator_info(size_t priority, associativity assoc);
        
        size_t priority;
        associativity assoc;
    };
    struct rule {
        rule(std::vector<std::string>&& body, std::string&& code, std::string&& res_type);
        
        std::vector<std::string> body;
        std::string code;
        std::string res_type;
    };

    grammar_info(grammar_info&& info);
    grammar_info(const grammar_info& info) = default;
    grammar_info();
    
    void clear();
    
    void calculate_LL1_info();
    
    friend std::ostream& operator << (std::ostream& out, const grammar_info& info);
    
    std::shared_ptr<LL1_info> ll1_info = nullptr;
    std::string start_not_terminal = "";
    
    std::map<std::string, token> tokens;
    std::map<std::string, operator_info> operators;
    std::string files_to_include, union_values;
    std::multimap<std::string, rule> rules;
    std::map<std::string, std::string> not_terminals_types;
};

class grammar_info_builder {
public:
    static const int DEFAULT_NEXT_TOKEN_ID = 256;
    
    void clear();
    grammar_info extract();
    
    void add_token(const std::string& name, const std::string& type);
    void add_code(std::string&& code);
    void add_union_values(std::string&& code);
    void add_operator(const std::string& name, grammar_info::associativity a);
    void add_rule(const std::string& name, std::vector<std::string>&& body,
                  std::string&& code, std::string&& res_type);
    void add_not_terminal(const std::string& name, const std::string& type);
    
    int next_token_id = DEFAULT_NEXT_TOKEN_ID;
    grammar_info res;
};

#endif // GRAMMAR_INFO_H
