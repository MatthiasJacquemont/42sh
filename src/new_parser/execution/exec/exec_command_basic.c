#include "exec_command_basic.h"

#include <ctype.h>
#include <err.h>
#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../../../builtins/builtins.h"
#include "../../../free_ast.h"
#include "../../../general.h"
#include "../../../variable.h"
#include "../ast_execution.h"

#define NB_BUILTIN 3

/* ========================================================================== **
** ---------------------------------- VAR ----------------------------------- **
** ========================================================================== */

char *find_var(char *name, struct general *general)
{
    struct var_list *ele = general->var_list;

    for (; ele != NULL; ele = ele->next)
    {
        if (strcmp(name, ele->name) == 0)
        {
            char *value = my_calloc(1, strlen(ele->value) + 3, general);
            strcpy(value, ele->value);
            return value;
        }
    }

    char *val = my_calloc(1, 1, general);
    return val;
}

char *replace_word(char *string, char *var, char *value,
                   struct general *general)
{
    char *dest = my_malloc(500, general);
    char *ptr;

    strcpy(dest, string);

    ptr = strstr(dest, var);
    if (ptr)
    {
        memmove(ptr + strlen(value), ptr + strlen(var),
                strlen(ptr + strlen(var)) + 1);
        strncpy(ptr, value, strlen(value));
    }

    return dest;
}

char *replace_variable(char *string, struct general *general)
{
    int i = 0;
    int scope = 0;

    int index_var;

    size_t string_size = strlen(string);

    char *temp_var = my_calloc(string_size + 5, 1, general);

    while (string[i] != '\0')
    {
        if (string[i] == '\'')
            scope = !scope;

        if (string[i] == '$' && scope == 0)
        {
            index_var = i;
            int j = 0;
            while (string[i] != '\0' && string[i] != ' ' && string[i] != '}')
            {
                if (string[i] == '$' || string[i] == '{')
                {
                    i++;
                    continue;
                }
                temp_var[j++] = string[i++];
            }
            string[index_var] = '\0';

            char *value = find_var(temp_var, general);

            if (string[i] == '}')
                i++;

            char *string_rest = my_calloc(string_size + 1000, 1, general);

            strcpy(string_rest, string + i);

            char *new_string =
                my_calloc(string_size + strlen(value) + 500, 1, general);
            strcpy(new_string, string);

            my_free(string, general);

            string = new_string;

            strcat(string, value);

            strcat(string, string_rest);

            /*
            char *dollar = my_calloc(1, strlen(temp_var) + 3, general);
            strcpy(dollar, "$");
            dollar = strcat(dollar, temp_var);

            char *new_string = replace_word(string, dollar, value, general);

            my_free(string, general);

            string = new_string;
            */
        }

        if (string[i] == '\0')
            break;
        i++;
    }

    return string;
}

void jump_var(char *string, struct general *general)
{
    int index = 0;
    int jump;
    int scope = 0;

    while (string[index] != '\0')
    {
        if (string[index] == '\'')
        {
            scope = !scope;
            index++;
            continue;
        }

        if (scope == 1)
        {
            index++;
            continue;
        }
        jump = get_variable(string + index, general);
        if (string[index + jump] == '\0')
            break;
        index += jump + 1;
    }
}

/* ========================================================================== **
** -------------------------------- Commands -------------------------------- **
** ========================================================================== */
char **create_args_list(char *string, struct general *general)
{
    char **args_list =
        my_calloc(strlen(string) + 4, strlen(string) + 4, general);

    int i = 0;
    int nb_args = 0;

    char stop = '\0';

    while (string[i] != '\0')
    {
        char *arg = my_calloc(strlen(string) + 1, 1, general);

        while (string[i] != '\0' && isspace(string[i]))
            i++;

        int i_arg = 0;
        while (string[i] != '\0' && (!isspace(string[i]) || stop != '\0'))
        {
            if (string[i] == '\'')
            {
                if (stop == '\0')
                {
                    stop = '\'';
                    i++;
                }
                else
                {
                    stop = '\0';
                    i++;
                }
            }
            if (string[i] == '"')
            {
                if (stop == '\0')
                {
                    stop = '"';
                    i++;
                }
                else
                {
                    stop = '\0';
                    i++;
                }
            }

            arg[i_arg++] = string[i];

            if (string[i] != '\0')
                i++;
        }

        args_list[nb_args++] = arg;
    }

    args_list[nb_args++] = NULL;

    return args_list;
}

int exec_command(struct node *node, struct general *general)
{
    if (!general || !node)
        return 2;

    struct node_command *node_builtin = node->structure;
    char *str = my_calloc(strlen(node_builtin->command) + 1, 1, general);
    strcpy(str, node_builtin->command);

    while (*str != '\0' && isspace(*str))
        str++;

    // jump_var(node_builtin->command, general); // Create var
    jump_var(str, general);
    while (contains_var(str))
    {
        str = replace_variable(str, general);
        jump_var(str, general); // Create var
        // jump_var(node_builtin->command, general); // Create var
    }
    if (is_variable(str))
        return 0;

    char *builtin = my_calloc(1, strlen(str) + 1, general);
    size_t index = 0;
    char *cursor = str;

    while (*cursor != '\0' && *cursor != ' ')
        builtin[index++] = *(cursor++);

    builtin[index++] = '\0';

    if (strcmp("echo", builtin) == 0)
    {
        my_free(builtin, general);
        return echo(str, general); // echo(cursor);
    }

    else if (strcmp("cd", builtin) == 0)
    {
        my_free(builtin, general);
        return cd(str, general, 1);
    }

    else if (strcmp("exit", builtin) == 0)
    {
        my_free(builtin, general);
        return builtin_exit(str, general);
    }

    else
    {
        char **args_list = create_args_list(str, general);

        pid_t pid = fork();
        int status = 1;

        if (pid == -1)
        {
            warn("Error when forking");
            return 1;
        }

        if (pid == 0)
        {
            // struct node_command *node_command = node->structure;
            // execl("/bin/sh", "42sh", "-c", node_command->command, NULL);
            // execl("/bin/sh", "42sh", "-c", str, NULL);
            // printf("%s %s\n", builtin, str + strlen(builtin));
            // execlp(builtin, builtin, str + strlen(builtin) + 1, NULL);

            // char **args_list = create_args_list(str, general);

            int res = execvp(builtin, args_list);
            warn("%s", builtin);
            exit(res);
        }
        else
        {
            int w = wait(&status); // waitpid(pid, &status, 0);
            // printf("status : %d, pid : %d, w = %d\n", WEXITSTATUS(status),
            // pid, w);
            if (w == -1)
                return WEXITSTATUS(status);
        }
        my_free(str, general);

        return WEXITSTATUS(status);
    }
}
/*
int exec_builtin(struct node *node)
{
    struct node_command *node_builtin = node->structure;

    char *builtin = calloc(1, 200);

    size_t index = 0;

    char *cursor = node_builtin->command;

    while (*cursor != '\0' && *cursor != ' ')
        builtin[index++] = *(cursor++);

    builtin[index++] = '\0';

    if (strcmp("echo", builtin) == 0)
    {
        echo(cursor);
    }

    return 1;

}
*/
int exec_if(struct node *node, struct general *general)
{
    if (general == NULL)
        return 12;

    struct node_if *node_if = node->structure;

    int res = execution(node_if->condition, general);
    if (res == 0)
        res = execution(node_if->node_then, general);
    else
        res = execution(node_if->node_else, general);

    return res;
}

int exec_while(struct node *node, struct general *general)
{
    if (general == NULL)
        return 12;

    struct node_while *node_while = node->structure;

    int res;

    while (execution(node_while->condition, general) == 0)
    {
        res = execution(node_while->node_then, general);
    }

    return res;
}

int exec_until(struct node *node, struct general *general)
{
    if (general == NULL)
        return 12;

    struct node_while *node_while = node->structure;

    int res;

    while ((!execution(node_while->condition, general)) == 0)
    {
        res = execution(node_while->node_then, general);
    }

    return res;
}

int exec_not(struct node *node, struct general *general)
{
    if (general == NULL)
        return 12;

    struct node_not *node_not = node->structure;

    int res = execution(node_not->child, general);

    return !res;
}

int exec_and(struct node *node, struct general *general)
{
    if (general == NULL)
        return 12;
    struct node_and_or *node_and = node->structure;

    int res = !execution(node_and->left_child, general)
        && !execution(node_and->right_child, general);

    return !res;
}

int exec_or(struct node *node, struct general *general)
{
    if (general == NULL)
        return 12;
    struct node_and_or *node_or = node->structure;

    int res = !execution(node_or->left_child, general)
        || !execution(node_or->right_child, general);

    return !res;
}
