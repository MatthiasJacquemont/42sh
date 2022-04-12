//#define _GNU_SOURCE

#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../free_ast.h"
#include "lexer.h"

int file_to_string(char *filename, char **string)
{
    FILE *file;

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    file = fopen(filename, "r");

    if (file == NULL)
        return 1;

    ssize_t alloc_size = 512;

    ssize_t size = 0;

    while ((nread = getline(&line, &len, file)) != -1)
    {
        size += nread;

        if (size > alloc_size)
        {
            while (size > alloc_size)
                alloc_size *= 2;
            *string = realloc(*string, alloc_size);
            if (*string == NULL)
                return 1;
        }

        // *(line + strlen(line) - 1) = ';';

        strcat(*string, line);
    }

    free(line);
    fclose(file);

    return 0;
}

int input_to_string(char **string)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    // int nb_line = 0;

    // printf("$ ");
    while ((nread = getline(&line, &len, stdin)) != -1)
    {
        /*
        if (line[0] == EOF || nread == -1)
        {
            free(line);
            //printf("\n");
            return 2;
        }
        */

        if (nread == -1 || line == NULL)
        {
            free(line);
            return 2;
        }

        ssize_t alloc_size = 512;

        int re_alloc = 0;

        if (nread > alloc_size)
        {
            while (nread > alloc_size - 5)
            {
                alloc_size *= 2;
                re_alloc = 1;
            }
            if (re_alloc)
                *string = realloc(*string, alloc_size);
            if (*string == NULL)
                return 1;
        }
        /*
        if (nb_line == 0)
            strncat(*string, line, strlen(line) - 1);
        else
            strncat(*string, line, strlen(line));
        nb_line++;
        */
        strcat(*string, line);
    }

    // printf("String is: %snext\n", *string);

    // strncpy(*string, line, strlen(line) - 1);

    // printf("%s\n", *string);

    free(line);

    return 0;
}

int args_to_string(int argc, char *argv[], char **string,
                   struct general *general)
{
    int res;

    if (argc == 1)
    {
        // Reading from standart input
        ssize_t alloc_size = 512;
        *string = my_calloc(1, alloc_size, general);

        if ((res = input_to_string(string)) == 2)
            return 2;
        if (res == 1)
            warn("Invalid argument");

        return -1;
    }

    else if (argc == 2)
    {
        // Reading from file given in arguments
        ssize_t alloc_size = 512;
        *string = my_calloc(1, alloc_size, general);
        if (file_to_string(argv[1], string) == 1)
        {
            warn("Can't open %s", argv[1]);
            my_free(*string, general);
            return 1;
        }

        if (*string == NULL || strlen(*string) == 0)
            return 18;
    }

    else if (strcmp(argv[1], "-c") == 0 && argc == 3)
    {
        // Reading from string in the arguments
        // Call lexer on string
        *string = my_calloc(1, strlen(argv[2]) + 2, general);
        strcpy(*string, argv[2]);
    }

    else
    {
        warnx("Invalid input");
        return 1;
    }

    return 0;
}
