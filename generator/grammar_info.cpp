#include "grammar_info.h"
using namespace std;

grammar_info::token::token(int val, const std::string& type)
: val(val), type(type) {
}
grammar_info::operator_info::operator_info(size_t priority, associativity assoc)
: priority(priority), assoc(assoc) {
}
grammar_info::rule::rule(std::vector<std::string>&& body, std::string&& code, std::string&& res_type)
: body(move(body)), code(move(code)), res_type(move(res_type)) {
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
        out << "  " << w.first << " " << w.second.priority << " " << w.second.assoc << "\n";
    }
    out << "rules:\n";
    for (const pair<string, grammar_info::rule>& w : info.rules) {
        out << "  " << w.first << ": ";
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
void grammar_info_builder::add_code(std::string&& code) {
    res.files_to_include = move(code);
}
void grammar_info_builder::add_operator(const std::string& name, grammar_info::associativity a) {
    res.operators.insert({name, {res.operators.size(), a}});
}
void grammar_info_builder::add_rule(const std::string& name, std::vector<std::string>&& body,
              std::string&& code, std::string&& res_type) {
    res.rules.insert({name, {move(body), move(code), move(res_type)}});
}
