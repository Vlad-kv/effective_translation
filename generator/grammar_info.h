#ifndef GRAMMAR_INFO_H
#define GRAMMAR_INFO_H

#include <iostream>
#include <map>
#include <string>
#include <vector>

class grammar_info {
public:
    struct token {
        token(int val, const std::string& type);
        
        int val;
        std::string type;
    };
    struct operator_info {
        enum associativity{left, right};
        
        int priority;
        associativity assoc;
    };
    struct rule {
        std::vector<std::string> body;
        std::string code;
        std::string res_type;
    };

    grammar_info(grammar_info&& info);
    grammar_info(const grammar_info& info);
    grammar_info();
    
    void clear();
    
    friend std::ostream& operator << (std::ostream& out, const grammar_info& info);
    
    std::map<std::string, token> tokens;
    std::map<std::string, operator_info> operators;
    std::string files_to_include;
    std::multimap<std::string, rule> rules;
};

class grammar_info_builder {
public:
    void clear();
    grammar_info extract();
    
    void add_token(const std::string& name, const std::string& type);
    
    int next_token_id = 256;
    grammar_info res;
};

#endif // GRAMMAR_INFO_H
