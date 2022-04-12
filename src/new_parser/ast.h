#ifndef AST_H
#define AST_H

#include "../general.h"
#include "../lexer/lexer.h"

enum node_type
{
    NODE_IF,
    NODE_WORD,
    NODE_COMMAND,
    NODE_ROOT,
    NODE_WHILE,
    NODE_NOT,
    NODE_AND,
    NODE_OR,
    NODE_UNTIL,
    NODE_CREATE_VAR,
    NODE_VAR
};

struct node
{
    enum node_type type;

    struct node *next;

    void *structure;
};

struct node_command
{
    char *command;
};

struct node_if
{
    struct node *condition;

    struct node *node_then;

    struct node *node_else;
};

struct node_while
{
    struct node *condition;

    struct node *node_then;
};

struct node_not
{
    struct node *child;
};

struct node_and_or
{
    struct node *left_child;

    struct node *right_child;
};

struct redirection
{
    char *type;

    struct node *child;
};

struct function
{
    enum node_type type;
    int (*fun)(struct node *node, struct general *general);
};

struct free_function
{
    enum node_type type;
    int (*fun)(struct node *node, struct general *general);
};

struct node *parser(char *str, struct general *general);

#endif /* !AST_H */
