#ifndef LEXER_H
#define LEXER_H

#include "../free_ast.h"

enum token_type
{
    EMPTY,
    NOT, // 11
    AND, // 12
    OR, // 13
    WORD, // 18
    IF, // 0
    THEN, // 1
    ELIF, // 2
    ELSE, // 3
    FI, // 4
    SEMICOLON, // 5
    FOR, // 6
    WHILE, // 7
    UNTIL, // 8
    DO, // 9
    DONE, // 10
    BRACKET_OPEN, // 14
    BRACKET_CLOSE, // 15
    EOL // 16
    // VAR, // 17
};

struct token
{
    char *str;
    enum token_type type;
};

int args_to_string(int argc, char *argv[], char **string,
                   struct general *general);
struct token *lexer(char *str, struct general *general);

#endif /* !LEXER_H */
