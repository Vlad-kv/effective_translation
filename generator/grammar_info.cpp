#include "grammar_info.h"
using namespace std;

grammar_info::token::token(int val, const std::string& type)
: val(val), type(type) {
}

grammar_info::grammar_info(grammar_info&& info)
: tokens(move(info.tokens)), operators(move(info.operators)),
  files_to_include(move(info.files_to_include)), rules(move(info.rules))  {
}
grammar_info::grammar_info(const grammar_info& info)
: tokens(info.tokens), operators(info.operators),
  files_to_include(info.files_to_include), rules(info.rules)  {
}
grammar_info::grammar_info() {
}

void grammar_info::clear() {
    tokens.clear();
    operators.clear();
    files_to_include.clear();
    rules.clear();
}
std::ostream& operator << (std::ostream& out, const grammar_info& info) {
    out << "files_to_include:\n  " << info.files_to_include << "\n";
    out << "tokens:\n";
    for (const pair<string, grammar_info::token>& w : info.tokens) {
        out << "  " << w.first << " " << w.second.val << " " << w.second.type << "\n";
    }
    out << "operators:\n";
    for (const pair<string, grammar_info::operator_info>& w : info.operators) {
        out << w.first << " " << w.second.priority << " " << w.second.assoc << "\n";
    }
    out << "rules:\n";
    for (const pair<string, grammar_info::rule>& w : info.rules) {
        out << w.first << ": ";
        for (const string &str : w.second.body) {
            out << str << " ";
        }
        out << w.second.code << " " << w.second.res_type << "\n";
    }
    return out;
}

void grammar_info_builder::clear() {
    next_token_id = 0;
    res.clear();
}
grammar_info grammar_info_builder::extract() {
    grammar_info result(move(res));
    clear();
    return result;
}
void grammar_info_builder::add_token(const std::string& name, const std::string& type) {
    int id = (name[0] == '\'')? name[1] : (next_token_id++);
    res.tokens.insert({name, {id, type}});
}
