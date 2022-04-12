#include <ctype.h>
#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "free_ast.h"
#include "general.h"

void create_var(char *string, struct general *general);
void print_var(struct general *general);

int contains_var(char *string)
{
    int index = 0;
    int scoping = 0;

    while (*string != '\0')
    {
        if (*string == '\'' && (index == 0 || *(string - 1) != '\\'))
            scoping = !scoping;

        if (*string == '$' && scoping == 0)
            return 1;

        string++;
        index++;
    }

    return 0;
}

int is_variable(char *string)
{
    /*
    int index = 0;
    int scoping = 0;

    while (*string != '\0')
    {
        if (*string == '\'' && (index == 0 || *(string-1) != '\\'))
            scoping = (scoping > 0) ? 0 : 1;

        string++;
        index++;
    }
    */

    /*
    if (fnmatch("[^0-9][^ ;]*=[^ ;]*", string, 0) == 0)
        return 1;
    */
    if (fnmatch("[A-Za-z_][A-Za-z_0-9]*=[^ ;]*", string, 0) == 0
        || fnmatch("[A-Za-z_][A-Za-z_0-9]*=", string, 0) == 0)
        return 1;

    return 0;
}

int get_variable(char *string, struct general *general)
{
    char *variable = my_calloc(strlen(string) + 3, 1, general);

    int i = 0;

    while (string[i] != '\0' && string[i] != ';' && string[i] != ' ')
    {
        variable[i] = string[i];
        i++;
    }

    if (string[i] != '\0' && string[i + 1] != '\0')
        i++;

    // strncpy(variable, string, i - 1);

    if (is_variable(variable))
    {
        create_var(variable, general);
        //        print_var(general); // DELETEEEE
    }

    // my_free(variable, general);

    return i;
}

void print_var(struct general *general)
{
    struct var_list *ele = general->var_list;

    for (; ele != NULL; ele = ele->next)
    {
        printf("%s = %s\n", ele->name, ele->value);
    }
}

static int search_var(char *value, char *name, struct general *general)
{
    struct var_list *ele = general->var_list;
    for (; ele != NULL; ele = ele->next)
    {
        if (strcmp(ele->name, name) == 0)
        {
            my_free(ele->value, general);
            ele->value = value;
            return 1; // 1 Variable was found and has been replaced
        }
    }
    return 0; // 0 No variable with same name was found
}

void add_to_var_list(char *value, char *name, struct general *general)
{
    if (search_var(value, name, general) == 1)
        return;

    struct var_list *ele = general->var_list;

    struct var_list *new_var = my_calloc(sizeof(struct var_list), 1, general);

    new_var->name = name;
    new_var->value = value;

    if (general->var_list == NULL)
    {
        general->var_list = new_var;
        return;
    }

    for (; ele != NULL && ele->next != NULL; ele = ele->next)
        continue;

    ele->next = new_var;
}

void create_var(char *string, struct general *general)
{
    char *name = my_calloc(1, strlen(string) + 3, general);
    char *value = my_calloc(1, strlen(string) + 3, general);

    size_t index = 0;

    while (*string != '=')
    {
        name[index++] = *string;
        string++;
    }

    string++;
    index = 0;

    char stop = '\0';

    while (*string != '\0' && !isspace(*string) && *string != ';')
    {
        if (*string == '\'')
        {
            if (stop == '\0')
            {
                stop = '\'';
                string++;
            }
            else if (stop == '\'')
            {
                stop = '\0';
                string++;
            }
        }
        if (*string == '"')
        {
            if (stop == '\0')
            {
                stop = '"';
                string++;
            }
            else if (stop == '"')
            {
                stop = '\0';
                string++;
            }
        }

        value[index++] = *string;
        string++;
    }

    add_to_var_list(value, name, general);
}
