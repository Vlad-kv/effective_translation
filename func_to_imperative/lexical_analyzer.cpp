#include <stdio.h>
#include <ctype.h>

#include <string>
#include <iostream>

#include "functional_parser.tab.hpp"

int yylex () {
    int c;

    while (((c = getchar ()) == ' ') || (c == '\t')) {
        continue;
    }
    if (c == '.' || isdigit (c)) {
        ungetc (c, stdin);
        scanf ("%lf", &yylval);
        return NUM;
    }
    if (c == EOF) {
        return 0;
    }
    return c;
}
