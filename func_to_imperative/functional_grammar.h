#ifndef FUNCTIONAL_GRAMMAR_H
#define FUNCTIONAL_GRAMMAR_H

class f_type;

#include <memory>

typedef std::shared_ptr<f_type> f_type_sp;

class f_type {
public:
    static const std::string DEFAULT_VAL;
    f_type(f_type_sp t_1, f_type_sp t_2, std::string val = DEFAULT_VAL);
    f_type(std::string str);
    
    static f_type_sp create(std::string str);
private:
    friend void loc_to_string(f_type_sp type, std::string& res, bool is_left);
    friend f_type_sp to_f_type(std::string input, int pos);
    
    f_type_sp t_1, t_2;
    std::string val;
};

std::string to_string(f_type_sp type);

#endif // FUNCTIONAL_GRAMMAR_H
