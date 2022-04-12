#ifndef GENERAL_H
#define GENERAL_H

struct general
{
    int fd;
    char *prev_path;
    char *path;
    struct free_list *free_list;
    struct var_list *var_list;
};

struct free_list
{
    int need_free;
    void *data;
    struct free_list *next;
};

struct var_list
{
    char *name;
    char *value;
    struct var_list *next;
};

#endif /* !GENERAL_H */
