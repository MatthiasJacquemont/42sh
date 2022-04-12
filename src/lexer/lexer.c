#include "lexer.h"

#include <ctype.h>
#include <err.h>
#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../free_ast.h"

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
        { "}", BRACKET_CLOSE }, { "\n", EOL },
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

struct token get_next_token(char *str, char next_char, struct general *general)
{
    struct token tok = { .str = "", .type = WORD };
    if (!str || *str == '\0')
        return tok;

    while ((isspace(*str) || *str == ';') && *str != '\0')
        str++;
    size_t l = 0;
    while (!isspace(str[l]) && str[l] != ';' && str[l] != '\0')
        l++;

    char *name = my_calloc(strlen(str) + 2, 1, general);
    strncpy(name, str, l);
    tok.str = name;

    static struct token models[] = {
        { "if", IF },           { "then", THEN },   { "elif", ELIF },
        { "else", ELSE },       { "fi", FI },       { ";", SEMICOLON },
        { "for", FOR },         { "while", WHILE }, { "do", DO },
        { "done", DONE },       { "until", UNTIL }, { "!", NOT },
        { "&&", AND },          { "||", OR },       { "{", BRACKET_OPEN },
        { "}", BRACKET_CLOSE }, { "\n", EOL },
    };
    // { "'", S_QUOTE } };

    for (unsigned i = 0; i < (sizeof(models) / sizeof(*models)); i++)
    {
        // if (strcmp(models[i].str, str) == 0 && (isspace(next_char) ||
        // next_char == ';' || next_char == 0))
        if (fnmatch(models[i].str, name, 0) == 0
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

int update_scoping(int scoping, char *current_token, char next_char, char stop)
{
    // printf("%c\n", next_char);
    if (stop != '\0'
        || (!isspace(next_char) && next_char != ';' && next_char != '\0'))
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
    char current;

    int scoping = 0;
    int elif = 0;

    while ((current = str[str_index]) != '\0')
    {
        if (skip_spaces(str, &str_index, stop, scoping))
            continue;

        if (current == '\n' && stop == '\0')
        {
            if (get_token(current_token, current, general).type == WORD)
            {
                int ind = str_index;
                while (ind > 0 && (isspace(str[ind]) || str[ind] == '\n'))
                    ind--;
                if (str[ind] != ';')
                    str[str_index] = ';';
                else
                    str_index++;
                continue;
            }
            else
                str[str_index] = ' ';
            continue;
        }

        if ((current == '\'' || current == '\"') && scoping < 2)
        {
            if (current == '\'')
            {
                if (stop == '\0')
                    stop = '\'';
                else if (stop == '\'')
                    stop = '\0';
            }
            // stop = (stop == '\'' ? '\0' : '\'');
            if (current == '"')
            {
                if (stop == '\0')
                    stop = '"';
                else if (stop == '"')
                    stop = '\0';
            }
            // stop = (stop == '"' ? '\0' : '"');

            current_token[cat_index++] = current;
            // current = str[++str_index];
            str_index++;
            continue;
        }
        if (skip_spaces(str, &str_index, stop, scoping))
            continue;

        if (str[str_index] == '{' && stop == '\0')
        {
            if (str_index > 0 && str[str_index - 1] == '$'
                && isspace(str[str_index + 1]))
            {
                warnx("Bad substitution");
                return NULL;
            }
            str_index++;
            scoping += 2;
            continue;
        }
        else if (str[str_index] == '}' && stop == '\0')
        {
            if (strlen(current_token) > 0 && str[str_index - 1] == ' ')
            {
                int k = strlen(current_token) - 1;
                while (k >= 0 && isspace(current_token[k]))
                    k--;

                if ((k >= 0 && current_token[k] != ';') || k < 0)
                {
                    warnx("Expected ';' before '}'");
                    return NULL;
                }
            }
            /*
            if (fnmatch("?*;[ ]*", current_token, 0) != 0)
            {
                warnx("Expected ';' before '}'");
                return NULL;
            }
            */
            str_index++;
            scoping -= 2;
            continue;
        }
        if (skip_spaces(str, &str_index, stop, scoping))
            continue;
        scoping = update_scoping(scoping, current_token, current, stop);

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
        /*
        if ((tok.type == IF || tok.type == WHILE || tok.type == UNTIL)
            // && stop != '\'')
            && stop == '\0')
            scoping++;
        else if (str[str_index] == '{' && stop == '\0') // stop != '\'')
        {
            str_index++;
            scoping += 2;
            continue;
        }
        else if ((str[str_index] == ';' || str[str_index] == ' ') &&
        str[str_index + 1] == 'f' && str[str_index + 2] == 'i') scoping--; else
        if ((str[str_index] == ';' || str[str_index] == ' ') && str[str_index +
        1] == 'n' && str[str_index + 2] == 'e'
                 && current_token[strlen(current_token)] == 'o' &&
        current_token[strlen(current_token) - 1] == 'd'
            )
            scoping--;
        else if (str[str_index] == '}' && stop == '\0') // stop != '\'')
        {
            str_index++;
            scoping -= 2;
            continue;
        }
        */

        if (skip_spaces(str, &str_index, stop, scoping))
            continue;

        if (scoping == 1
            // && (tok.type == WORD || tok.type == NOT || tok.type == AND)
            && (tok.type > EMPTY && tok.type <= WORD)
            // && and_or(str, str_index)
            // && get_next_token(str + str_index, '\0', general).type == WORD)
            && get_next_token(str + str_index, '\0', general).type > EMPTY
            && get_next_token(str + str_index, '\0', general).type <= WORD)
        {
            current_token[cat_index++] = current;
            str_index++;
            continue;
        }
        else if ((((tok.type != WORD /*&& tok.type != NOT*/) || current == ';'
                   || and_or(str, str_index)) // || current == '\n'))
                  && stop == '\0' && scoping < 2)
                 // || (scoping == 1 && get_next_token(str + str_index, '\0',
                 // general).type != WORD)
        )
        // && stop != '\'' && scoping < 2)
        {
            tokens[tok_index++] =
                get_token(current_token, str[str_index], general);
            if (elif > 0 && fnmatch("?*[ ;]fi", current_token, 0) == 0)
            {
                tokens[tok_index++] = get_token("fi", ' ', general);
                scoping--;
                elif--;
            }
            memset(current_token, 0, strlen(str) + 1);
            cat_index = 0;

            while (current == ' ' || current == ';') // || current == '\n')
            {
                /*
                                if (current == '\n' && tokens[tok_index -
                   1].type == WORD)
                                {
                                    str[str_index] = ';';
                                    break;
                                }
                                else if (current == '\n' && tokens[tok_index -
                   1].type != WORD)
                                {
                                    str[str_index] = ' ';
                                }
                */
                current = str[++str_index];
            }
            my_free(tok.str, general);
            continue;
        }
        else
            my_free(tok.str, general);

        if (skip_spaces(str, &str_index, stop, scoping))
            continue;

        current_token[cat_index++] = current;
        str_index++;
        continue;
    }

    if (strlen(current_token) > 0)
    {
        while (*current_token != '\0' && isspace(*current_token))
            current_token++;

        struct token tok = get_token(current_token, str[str_index], general);
        scoping = update_scoping(scoping, current_token, current, stop);
        /*
        if (tok.type == FI && stop == '\0')
            --scoping;
        */
        tokens[tok_index++] = tok;
        if (elif > 0 && fnmatch("?*[ ;]fi", current_token, 0) == 0)
        {
            tokens[tok_index++] = get_token("fi", ' ', general);
            scoping--;
            elif--;
        }
    }
    my_free(current_token, general);
    my_free(str, general);

    if (scoping != 0)
    {
        warnx("if/fi mismatch (scoping == %d)", scoping);
        return NULL;
    }
    if (stop != '\0')
    {
        warnx("Quotes mismatch (%c)", stop);
        return NULL;
    }

    // return ((scoping == 0) ? tokens : NULL);
    return tokens;
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
