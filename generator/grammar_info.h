#ifndef GRAMMAR_INFO_H
#define GRAMMAR_INFO_H

#include <map>
#include <string>
#include <vector>

class grammar_info {
public:
    struct token {
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
    
    std::map<std::string, token> tokens;
    std::map<std::string, operator_info> operators;
    std::string files_to_include;
    std::map<std::string, rule> rules;
};

#endif // GRAMMAR_INFO_H
