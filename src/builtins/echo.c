#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../general.h"

int echo(char *str, struct general *general)
{
    if (!general)
        return 1;

    if (!str || strlen(str) < 5)
    {
        printf("\n");
        return 0;
    }

    while (*str == ' ')
        str++;
    if (strlen(str) < 4)
        return 1;
    str += 4;

    while (*str == ' ')
        str++;

    char stop = ';';
    int interprets = 0;
    char *new_line = "\n";

    while (1)
    {
        while (*str == ' ')
            str++;

        if (*str == '-' && *(str + 1) != '\0'
            && (*(str + 2) == ' ' || *(str + 2) == '\0'))
        {
            if (*(str + 1) == 'n')
            {
                new_line = "";
                str += 2;
            }
            else if (*(str + 1) == 'e')
            {
                interprets = 1;
                str += 2;
            }
            else
                break;
        }
        else
            break;
    }

    while (*str == ' ')
        str++;

    char *res = calloc(1, strlen(str) + 1);
    int cat_index = 0;

    while (*str != '\0')
    {
        if (*str == ' ' && (*(str + 1) == ' ' || *(str + 1) == '\0')
            && stop == ';')
        {
            str++;
            continue;
        }

        if (interprets == 1 && *str == '\\' && stop != '\'')
        {
            str++;
            switch (*str)
            {
            case 'n':
                res[cat_index++] = '\n';
                str++;
                break;
            case 't':
                res[cat_index++] = '\t';
                str++;
                break;
            case '\\':
                res[cat_index++] = '\\';
                str++;
                break;
            }
        }
        /*
        else if (*str == '\\')
        {
            str++;
            continue;
        }
        */
        /*
                if (stop == ';' && strcmp(res, "ech") == 0 && *str == 'o')
                {
                    str++;
                    while (*str == ' ')
                        str++;

                    memset(res, 0, 5);
                    cat_index = 0;

                    if (*str == '\0')
                    {
                        printf("%s", new_line);
                        break;
                    }
                }

                if (stop == ';' && strcmp(res, "-") == 0
                    && (*str == 'n' || *str == 'e')
                    && (*(str + 1) == ' ' || *(str + 1) == ';'))
                {
                    if (*str == 'n')
                        new_line = "";
                    if (*str == 'e')
                        interprets = 1;
                    str++;

                    while (*str == ' ')
                        str++;

                    memset(res, 0, 3);
                    cat_index = 0;

                    if (*str == '\0')
                    {
                        printf("%s", new_line);
                        break;
                    }
                    continue;
                }
        */
        if (*str == '\'' || *str == '"')
        {
            /*
            if (stop != ';' && *str != stop)
                res[cat_index++] = *str;
            str++;
            if (stop == ';')
                stop = *(str - 1);
            else
            {
                if (*(str - 1) == stop)
                    stop = ';';
                continue;
            }
            */
            if (*str == '\'')
            {
                if (stop == *str)
                    stop = ';';
                else if (stop == ';')
                    stop = '\'';
                else
                    res[cat_index++] = *str;
            }
            else if (*str == '"')
            {
                if (stop == *str)
                    stop = ';';
                else if (stop == ';')
                    stop = '"';
                else
                    res[cat_index++] = *str;
            }
            str++;
            continue;
        }

        if (*str == ';' && stop == ';')
        {
            if (strlen(res) > 0)
            {
                printf("%s%s", res, new_line);
                memset(res, 0, strlen(str) + 1);
                cat_index = 0;
            }
            echo(++str, general);
            break;
        }

        res[cat_index++] = *str;
        while (*str == ' ' && *(str + 1) == ' ' && stop == ';')
            str++;
        str++;
    }

    // if (strlen(res) > 0)
    printf("%s%s", res, new_line);
    free(res);
    return 0;
}

/*
int main(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
        echo(argv[i]);

    // char *str = "echo -n";
    // char *pattern = "echo [-]?[]?";
    // printf("'%s' matches '%s' ? : %d\n", str, pattern, fnmatch(pattern, str,
0));

    return 0;
}
*/
