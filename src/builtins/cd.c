#include <err.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "../free_ast.h"
#include "../general.h"
#include "../variable.h"

int cd(char *path, struct general *general, int modify)
{
    if (general == NULL)
        return 12;

    if (path[0] == 'c' && path[1] == 'd')
        path += 2;

    while (*path == ' ')
        path++;

    for (size_t i = 0; path[i] != '\0'; i++)
    {
        if (path[i] == ' ')
        {
            path[i] = '\0';
            break;
        }
    }

    if (!path)
    {
        fprintf(stderr, "cd: Expected argument\n");
        return 2;
    }

    char *new_prev = calloc(1, 1000);
    if (*path == '-' && strlen(path) == 1)
    {
        if (strlen(general->prev_path) == 0)
            general->prev_path = getcwd(NULL, 0);

        if (chdir(general->prev_path) != 0)
        {
            warnx("cd: %s: No such file or directory", general->prev_path);
            return 2;
        }

        // printf("%s\n", general->prev_path);

        getcwd(new_prev, 1000);

        printf("%s\n", new_prev);

        if (strlen(general->prev_path) < strlen(general->path))
        {
            my_free(general->prev_path, general);
            general->prev_path =
                my_calloc(strlen(general->path) + 3, 1, general);
            // general->prev_path = realloc(general->prev_path, strlen(new_prev)
            // + 3);
        }

        strcpy(general->prev_path, general->path);

        // getcwd(new_prev, 1000);

        if (strlen(general->path) < strlen(new_prev))
        {
            my_free(general->path, general);
            general->path = my_calloc(strlen(new_prev) + 3, 1, general);
            // general->path = my_realloc(general->path, strlen(new_prev) + 3,
            // general);
        }

        strcpy(general->path, new_prev);

        free(new_prev);

        char *pwd = my_calloc(strlen(general->path) + 1, 1, general);
        char *oldpwd = my_calloc(strlen(general->prev_path) + 1, 1, general);

        strcpy(pwd, general->path);
        strcpy(oldpwd, general->prev_path);

        add_to_var_list(oldpwd, "OLDPWD", general);
        add_to_var_list(pwd, "PWD", general);

        return 0;
    }

    //    char *new_prev = getcwd(NULL, 0);
    getcwd(new_prev, 1000);

    if (modify && strlen(general->prev_path) < strlen(new_prev))
    {
        // general->prev_path = realloc(general->prev_path, strlen(new_prev) +
        // 3);
        my_free(general->prev_path, general);
        general->prev_path = my_calloc(strlen(new_prev) + 3, 1, general);
    }

    if (modify)
        strcpy(general->prev_path, new_prev);

    if (chdir(path) != 0)
    {
        warnx("cd: %s: No such file or directory", general->prev_path);
        free(new_prev);
        return 2;
    }

    getcwd(new_prev, 1000);

    if (strlen(general->path) < strlen(new_prev))
    {
        // general->path = realloc(general->path, strlen(new_prev) + 3);
        my_free(general->path, general);
        general->path = my_calloc(strlen(new_prev) + 3, 1, general);
    }

    strcpy(general->path, new_prev);

    free(new_prev);

    char *pwd = my_calloc(strlen(general->path) + 1, 1, general);
    char *oldpwd = my_calloc(strlen(general->prev_path) + 1, 1, general);

    strcpy(pwd, general->path);
    strcpy(oldpwd, general->prev_path);

    add_to_var_list(oldpwd, "OLDPWD", general);
    add_to_var_list(pwd, "PWD", general);

    return 0;
}

/*
int main(int argc, char *argv[])
{
    //char *buff;

    struct general *general = calloc(1, sizeof(struct general));

    char *path = calloc(1, 4);
    general->prev_path = path;

    for (int i = 1; i < argc; i++)
    {
        cd(argv[i], general);
        //buff = getcwd(NULL, 0);
        //printf("%s\n", buff);

    }
    return 0;
}
*/
