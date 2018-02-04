#include "grammar_info.h"
using namespace std;

grammar_info::grammar_info(grammar_info&& info)
: tokens(move(info.tokens)), operators(move(info.operators)),
  files_to_include(move(info.files_to_include)), rules(move(info.rules))  {
}

grammar_info::grammar_info(const grammar_info& info)
: tokens(info.tokens), operators(info.operators),
  files_to_include(info.files_to_include), rules(info.rules)  {
}
