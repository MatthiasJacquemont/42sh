#include <ctype.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../free_ast.h"
#include "../general.h"

int builtin_exit(char *str, struct general *general)
{
    if (general == NULL)
        return 1;

    while (*str == ' ')
        str++;
    if (strlen(str) < 4)
        return 1;
    str += 4;
    while (*str == ' ')
        str++;

    int l = 0;
    while (str[l] != '\0' && !isspace(str[l]) && str[l] != ';')
        l++;
    str[l] = '\0';

    int res = atoi(str);
    if (res < 0 || (*str > '9' || *str < '0'))
    {
        warnx("exit: Illegal number: %s", str);
        exit(2);
    }

    destroy(general->free_list);
    free(general);
    exit(res % 256);
}
