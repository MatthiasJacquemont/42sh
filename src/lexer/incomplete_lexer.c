#include <ctype.h>
#include <err.h>
#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../free_ast.h"
#include "lexer.h"

/* ========================================================================== **
** --------------------- Creates a token from a string ---------------------- **
** ========================================================================== */

struct token get_token(char *str, char next_char, struct general *general)
{
    struct token tok = { .str = "", .type = WORD };
    if (!str)
        return tok;

    char *name = my_calloc(strlen(str) + 2, 1, general);
    strcpy(name, str);

    tok.str = name;

    static struct token models[] = {
        { "if", IF },           { "then", THEN },   { "elif", ELIF },
        { "else", ELSE },       { "fi", FI },       { ";", SEMICOLON },
        { "for", FOR },         { "while", WHILE }, { "do", DO },
        { "done", DONE },       { "until", UNTIL }, { "!", NOT },
        { "&&", AND },          { "||", OR },       { "{", BRACKET_OPEN },
        { "}", BRACKET_CLOSE }, { "\n", EOL }
    };
    // { "'", S_QUOTE } };

    for (unsigned i = 0; i < (sizeof(models) / sizeof(*models)); i++)
    {
        // if (strcmp(models[i].str, str) == 0 && (isspace(next_char) ||
        // next_char == ';' || next_char == 0))
        if (fnmatch(models[i].str, str, 0) == 0
            && (isspace(next_char) || next_char == ';' || next_char == '\0'))
        {
            tok.type = models[i].type;
            break;
        }
    }

    return tok;
}

/* ========================================================================== **
** ----------------------- Testing for '&&' and '||' ------------------------ **
** ========================================================================== */

int and_or(char *str, int index)
{
    if (str[index + 1] == '&' && str[index + 2] == '&')
        return 1;
    if (str[index + 1] == '|' && str[index + 2] == '|')
        return 1;
    return 0;
}

/* ========================================================================== **
** ---------------------------- Skipping Spaces ----------------------------- **
** ========================================================================== */

int skip_spaces(char *str, int *str_index, char stop, int scoping)
{
    int cont = 0;
    while (str[*str_index] == ' ' && str[*str_index + 1] == ' ' && scoping <= 1
           && stop == '\0')
    {
        cont = 1;
        (*str_index)++;
    }

    return cont;
}

/* ========================================================================== **
** --------------------------------- Scoping -------------------------------- **
** ========================================================================== */

int update_scoping(int scoping, char *current_token, char stop)
{
    if (stop != '\0')
        return scoping;

    // INCREMENTING SCOPING
    if (fnmatch("if", current_token, 0) == 0
        || fnmatch("[ ;]*if", current_token, 0) == 0
        || fnmatch("?*[ ;]if", current_token, 0) == 0)
        scoping++;

    if (fnmatch("while", current_token, 0) == 0
        || fnmatch("[ ;]*while", current_token, 0) == 0
        || fnmatch("?*[ ;]while", current_token, 0) == 0)
        scoping++;

    if (fnmatch("until", current_token, 0) == 0
        || fnmatch("[ ;]*until", current_token, 0) == 0
        || fnmatch("?*[ ;]until", current_token, 0) == 0)
        scoping++;

    // DECREMENTING SCOPING
    if (fnmatch("fi", current_token, 0) == 0
        || fnmatch("[ ;]*fi", current_token, 0) == 0
        || fnmatch("?*[ ;]fi", current_token, 0) == 0)
        scoping--;

    if (fnmatch("done", current_token, 0) == 0
        || fnmatch("[ ;]*done", current_token, 0) == 0
        || fnmatch("?*[ ;]done", current_token, 0) == 0)
        scoping--;

    return scoping;
}

/* ========================================================================== **
** ------------------- Adding current tokens to tokens[] -------------------- **
** ========================================================================== */

static struct token *add_token(struct token *tokens, char *current_token,
                               char *full_token, char next_char,
                               struct general *general)
{
    int tok_index = 0;
    while (tokens[tok_index].str)
        tok_index++;

    if (strlen(full_token) > 0)
        tokens[tok_index++] = get_token(full_token, next_char, general);

    if (strlen(current_token) > 0)
        tokens[tok_index++] = get_token(current_token, next_char, general);

    return tokens;
}

/* ========================================================================== **
** --------- The lexer, returning an array of tokens from a string  --------- **
** ========================================================================== */

struct token *lexer(char *str, struct general *general)
{
    if (!str || strlen(str) <= 0)
    {
        my_free(str, general);
        return NULL;
    }

    struct token *tokens =
        my_calloc(strlen(str) + 1, sizeof(struct token), general);
    if (!tokens)
    {
        my_free(str, general);
        return NULL;
    }

    int str_index = 0;
    int tok_index = 0;
    int cat_index = 0;

    char stop = '\0';
    char *current_token = my_calloc(strlen(str) + 1, 1, general);
    char *full_token = my_calloc(strlen(str) + 1, 1, general);
    char current;

    int scoping = 0;
    int elif = 0;

    while ((current = str[str_index]) != '\0')
    {
        if (skip_spaces(str, &str_index, stop, scoping))
            continue;

        if ((current == '\'' || current == '\"') && scoping < 2)
        {
            if (current == '\'')
                stop = (stop == '\'' ? '\0' : '\'');
            if (current == '"')
                stop = (stop == '"' ? '\0' : '"');
            current_token[cat_index++] = current;
            current = str[++str_index];
        }
        if (skip_spaces(str, &str_index, stop, scoping))
            continue;

        if (str[str_index] == '{' && stop == '\0')
        {
            str_index++;
            scoping += 2;
            continue;
        }
        else if (str[str_index] == '}' && stop == '\0')
        {
            if (fnmatch("?*;[ ]*", current_token, 0) != 0)
            {
                warnx("Expected ';' before '}'");
                return NULL;
            }
            str_index++;
            scoping -= 2;
            continue;
        }
        if (skip_spaces(str, &str_index, stop, scoping))
            continue;
        scoping = update_scoping(scoping, current_token, stop);

        struct token tok = get_token(current_token, str[str_index], general);
        if (tok.type == ELIF)
        {
            tokens[tok_index++] = get_token("else", ' ', general);
            memset(current_token, 0, strlen(str) + 1);
            cat_index = 2;

            strcpy(current_token, "if ");

            scoping++;
            elif++;
        }

        // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

        if (skip_spaces(str, &str_index, stop, scoping))
            continue;
        if (((tok.type != WORD && tok.type != NOT)
             || current == ';' // || current == '\n'
             || and_or(str, str_index))
            && stop == '\0' && scoping < 2)
        {
            if (tok.type == WORD)
            {
                strcat(full_token, current_token);
                str_index++;
                continue;
            }
            else
                tokens = add_token(tokens, current_token, full_token, current,
                                   general);

            /*
            tokens[tok_index++] = get_token(current_token, str[str_index],
            general); if (elif > 0 && fnmatch("?*[ ;]fi", current_token, 0) ==
            0)
            {
                tokens[tok_index++] = get_token("fi", ' ', general);
                scoping--;
                elif--;
            }
            memset(current_token, 0, strlen(str) + 1);
            cat_index = 0;

            while (current == ' ' || current == ';')
                current = str[++str_index];
            my_free(tok.str, general);
            */

            continue;
        }
        else
            my_free(tok.str, general);
        /*
                if (skip_spaces(str, &str_index, stop, scoping)) continue;
                if (((tok.type != WORD && tok.type != NOT) || current == ';' //
           || current == '\n'
                     || and_or(str, str_index)) && stop == '\0' && scoping < 2)
                {
                    if (tok.type == WORD)
                    {
                        strcat(full_token, current_token);
                        str_index++;
                        continue;
                    }

                    tokens[tok_index++] = get_token(current_token,
           str[str_index], general); if (elif > 0 && fnmatch("?*[ ;]fi",
           current_token, 0) == 0)
                    {
                        tokens[tok_index++] = get_token("fi", ' ', general);
                        scoping--;
                        elif--;
                    }
                    memset(current_token, 0, strlen(str) + 1);
                    cat_index = 0;

                    while (current == ' ' || current == ';')
                        current = str[++str_index];
                    my_free(tok.str, general);
                    continue;
                }
                else
                    my_free(tok.str, general);
        */

        if (skip_spaces(str, &str_index, stop, scoping))
            continue;

        current_token[cat_index++] = current;
        str_index++;
        continue;
    }

    if (strlen(current_token) > 0)
    {
        struct token tok = get_token(current_token, str[str_index], general);
        scoping = update_scoping(scoping, current_token, stop);

        tokens[tok_index++] = tok;
        if (elif > 0 && fnmatch("?*[ ;]fi", current_token, 0) == 0)
        {
            tokens[tok_index++] = get_token("fi", ' ', general);
            scoping--;
            elif--;
        }
    }
    my_free(full_token, general);
    my_free(current_token, general);
    my_free(str, general);

    if (scoping != 0)
        warnx("if/fi mismatch (scoping == %d)", scoping);
    return ((scoping == 0) ? tokens : NULL);
}

/* ========================================================================== **
** ---------------- The old main function used for debugging ---------------- **
** ========================================================================== */

/*
int main(int argc, char *argv[])
{
    char *str_arg = "if echo Ceci est une condition toujours vraie; echo Ici
aussi; then echo Ceci sera toujours execute; else echo Ceci ne sera jamais
execute; fi"; if (argc > 1) str_arg = argv[1];

    char *str = my_calloc(1, strlen(str_arg) + 1, general);
    strcpy(str, str_arg);

    struct token *arr = lexer(str);

    printf("Array :\n");
    struct token *ptr = arr;
    if (!ptr)
        printf("\tNULL\n");

    while (ptr && ptr->str)
    {
        (ptr->type == EOL) ?
            printf("\t-> EOL (%d)\n", ptr->type) :
            printf("\t-> '%s' (%d)\n", ptr->str, ptr->type);
        free(ptr->str);
        ptr++;
    }

    free(arr);
}
*/
