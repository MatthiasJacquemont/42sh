#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../free_ast.h"
#include "../lexer/lexer.h"
#include "ast.h"

static void next_token(struct token **toks, int do_free,
                       struct general *general)
{
    if (do_free)
        my_free((*toks)->str, general);
    (*toks)++;
}

struct node *parser(char *str, struct general *general);
static struct node *if_token(struct token **toks, struct node *ast,
                             struct general *general)
{
    next_token(toks, 1, general);
    struct node_if *new_if = my_calloc(1, sizeof(struct node_if), general);

    int cond = 1;
    int _then = 0;
    int _else = 0;

    char *error = "then";

    // printf("If:");
    while (*toks && (*toks)->str && (*toks)->type != FI)
    {
        if (cond == 1) // && (*toks)->type == THEN)
        {
            if ((*toks)->type == THEN)
            {
                cond = 0;
                _then = 1;
                error = "fi";

                // printf("\nThen:");
                next_token(toks, 1, general);
                continue;
            }
            else
                new_if->condition = parser((*toks)->str, general);
        }
        if (_then == 1) // && (*toks)->type == ELSE)
        {
            if ((*toks)->type == ELSE)
            {
                _then = 2;
                _else = 1;
                // error = "fi";

                // printf("\nElse:");
                next_token(toks, 1);
                continue;
            }
            else
                new_if->node_then = parser((*toks)->str, general);
        }
        if (_else == 1)
        {
            new_if->node_else = parser((*toks)->str, general);
        }

        // printf("\n\t%s", (*toks)->str);
        next_token(toks, 0);
    }

    // printf("\n");

    while (ast->next) // && ast->type != NODE_COMMAND)
        ast = ast->next;

    ast->next = my_calloc(1, sizeof(struct node), general);
    ast->next->type = NODE_IF;
    ast->next->structure = new_if;

    if ((*toks)->type != FI || _then < 1)
    {
        warnx("'%s' operator was expected", error);
        // free_if(new_if);
        free_ast(ast, general);
        return NULL;
    }

    return ast;
}

static struct node *while_token(struct token **toks, struct node *ast,
                                struct general *general)
{
    next_token(toks, 1);
    struct node_if *new_while =
        my_calloc(1, sizeof(struct node_while), general);

    int cond = 1;
    int _do = 0;

    char *error = "do";

    while (*toks && (*toks)->str && (*toks)->type != DONE)
    {
        if (cond == 1)
        {
            if ((*toks)->type == DO)
            {
                cond = 0;
                _do = 1;
                error = "done";
                next_token(toks, 1);
                continue;
            }
            else
                new_while->condition = parser((*toks)->str, general);
        }
        if (_do == 1)
            new_while->node_then = parser((*toks)->str, general);

        next_token(toks, 0);
    }

    while (ast->next)
        ast = ast->next;

    ast->next = my_calloc(1, sizeof(struct node), general);
    ast->next->type = NODE_WHILE;
    ast->next->structure = new_while;

    if ((*toks)->type != DONE || _do < 1)
    {
        warnx("'%s' operator was expected", error);
        free_ast(ast, general);
        return NULL;
    }

    return ast;
}

static struct node *until_token(struct token **toks, struct node *ast,
                                struct general *general)
{
    next_token(toks, 1);
    struct node_if *new_until =
        my_calloc(1, sizeof(struct node_while), general);

    int cond = 1;
    int _do = 0;

    char *error = "do";

    while (*toks && (*toks)->str && (*toks)->type != DONE)
    {
        if (cond == 1)
        {
            if ((*toks)->type == DO)
            {
                cond = 0;
                _do = 1;
                error = "done";
                next_token(toks, 1);
                continue;
            }
            else
                new_until->condition = parser((*toks)->str, general);
        }
        if (_do == 1)
            new_until->node_then = parser((*toks)->str, general);

        next_token(toks, 0);
    }

    while (ast->next)
        ast = ast->next;

    ast->next = my_calloc(1, sizeof(struct node), general);
    ast->next->type = NODE_UNTIL;
    ast->next->structure = new_until;

    if ((*toks)->type != DONE || _do < 1)
    {
        warnx("'%s' operator was expected", error);
        free_ast(ast, general);
        return NULL;
    }

    return ast;
}

static struct node *not_token(struct token **toks, struct node *ast,
                              struct general *general)
{
    if (!ast)
        return NULL;
    struct node *node_start = ast;

    next_token(toks, 1);
    struct node_not *new_not = my_calloc(1, sizeof(struct node_not), general);
    new_not->child = parser((*toks)->str, general);

    while (ast->next) // && ast->type != NODE_COMMAND)
        ast = ast->next;

    if (ast->next)
        ast->structure = new_not;
    else
    {
        ast->next = my_calloc(1, sizeof(struct node), general);
        ast->next->type = NODE_NOT;
        ast->next->next = NULL;
        ast->next->structure = new_not;
    }

    return node_start;
}

static struct node *and_token(struct token **toks, struct node *ast,
                              struct general *general)
{
    if (!ast)
        return NULL;

    struct node *node_start = ast;

    struct node_and_or *new_and =
        my_calloc(1, sizeof(struct node_and_or), general);

    while (ast->next && ast->next->next)
        ast = ast->next;

    new_and->left_child = ast->next;
    next_token(toks, 1);
    new_and->right_child = parser((*toks)->str, general);

    ast->next = my_calloc(1, sizeof(struct node), general);
    ast->next->type = NODE_AND;
    ast->next->next = NULL;
    ast->next->structure = new_and;

    return node_start;
}

static struct node *or_token(struct token **toks, struct node *ast,
                             struct general *general)
{
    if (!ast)
        return NULL;

    struct node *node_start = ast;

    struct node_and_or *new_or =
        my_calloc(1, sizeof(struct node_and_or), general);

    while (ast->next && ast->next->next)
        ast = ast->next;

    new_or->left_child = ast->next;
    next_token(toks, 1);
    new_or->right_child = parser((*toks)->str, general);

    ast->next = my_calloc(1, sizeof(struct node), general);
    ast->next->type = NODE_OR;
    ast->next->next = NULL;
    ast->next->structure = new_or;

    return node_start;
}

static struct node *command_token(struct token **toks, struct node *ast,
                                  struct general *general)
{
    if (!ast)
        return NULL;

    struct node *node_start = ast;

    struct node_command *new_command =
        my_calloc(1, sizeof(struct node_command), general);
    char *command = my_calloc(1, strlen((*toks)->str) + 1, general);
    strcpy(command, (*toks)->str);
    new_command->command = command;

    while (ast->next) // && ast->type != NODE_COMMAND)
        ast = ast->next;

    if (ast->next)
        ast->structure = new_command;
    else
    {
        ast->next = my_calloc(1, sizeof(struct node), general);
        ast->next->type = NODE_COMMAND;
        ast->next->next = NULL;
        ast->next->structure = new_command;
    }

    return node_start;
}

/*
static void print_tokens(struct token *toks)
{
    printf("Tokens :");
    while (toks && toks->str)
    {
        if (toks->type != EOL)
            printf("\n\t(%d) (%s)", toks->type, toks->str);
        else
            printf("\n\t(%d) (EOL)", toks->type);
        toks++;
    }
    printf("\n\n");
}
*/
struct node *parser(char *str, struct general *general)
{
    struct token *tokens = lexer(str, general);
    if (!tokens)
        return NULL;

    // print_tokens(tokens);

    struct node *ast = my_calloc(1, sizeof(struct node), general);
    ast->type = NODE_ROOT;

    struct token *toks = tokens;
    while (toks && toks->str)
    {
        if (ast && toks->type == IF)
            ast = if_token(&toks, ast, general);

        else if (ast && toks->type == WHILE)
            ast = while_token(&toks, ast, general);

        else if (ast && toks->type == UNTIL)
            ast = until_token(&toks, ast, general);

        else if (ast && toks->type == AND)
        {
            ast = and_token(&toks, ast, general);
            next_token(&toks, 0);
            continue;
        }

        else if (ast && toks->type == OR)
        {
            ast = or_token(&toks, ast, general);
            next_token(&toks, 0);
            continue;
        }

        else if (ast && toks->type == NOT)
        {
            ast = not_token(&toks, ast, general);
            next_token(&toks, 0);
            continue;
        }

        else if (ast && toks->type == WORD)
            ast = command_token(&toks, ast);

        /*
        if (ast && toks->type == IF && (ast = if_token(&toks, ast)) == NULL)
        {
            // free(ast);
            free_ast(ast);
            free(ast);
            ast = NULL;
        }

        if (ast && toks->type == WORD && (ast = command_token(&toks, ast)) ==
        NULL)
        {
            // free(ast);
            free_ast(ast);
            free(ast);
            ast = NULL;
        }
        */

        next_token(&toks, 1);
    }

    // print_ast(ast);
    my_free(tokens, general);
    return ast;
}
