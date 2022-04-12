#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "general.h"
#include "lexer/lexer.h"
#include "new_parser/ast.h"

void print_list(struct free_list *free_list)
{
    for (struct free_list *ele = free_list; ele != NULL; ele = ele->next)
        printf("-> %d ", ele->need_free);
    printf("\n");
}

struct free_list *free_list_init(void)
{
    struct free_list *free_list = calloc(1, sizeof(struct free_list));

    return free_list;
}

void my_free(void *ptr, struct general *general)
{
    if (ptr == NULL || general == NULL)
        return;
    struct free_list *free_list = general->free_list;

    struct free_list *ele = free_list;
    for (; ele != NULL; ele = ele->next)
    {
        if (ele->need_free == 1)
        {
            if (ele->data == ptr)
            {
                free(ele->data);
                ele->need_free = 0;
                return;
            }
        }
    }

    // free(ptr);
}
/*
void destroy(struct free_list *free_list)
{
    struct free_list *ele = free_list;
    struct free_list *next;

    while (ele != NULL)
    {
        next = ele->next;
        free(ele);

        ele = next;
    }
}
*/

void destroy(struct free_list *free_list)
{
    if (free_list == NULL)
        return;

    destroy(free_list->next);

    if (free_list->need_free)
        free(free_list->data);
    free(free_list);

    free_list = NULL;
}

static void add_free_list(struct free_list *free_list, void *ptr)
{
    struct free_list *new = malloc(sizeof(struct free_list));

    new->data = ptr;
    new->need_free = 1;
    new->next = NULL;

    for (; free_list != NULL; free_list = free_list->next)
    {
        if (free_list->next == NULL)
        {
            free_list->next = new;
            break;
        }
    }
}

void *my_malloc(size_t size, struct general *general)
{
    void *res = malloc(size);

    if (res == NULL)
        return NULL;
    add_free_list(general->free_list, res);

    return res;
}

void *my_calloc(size_t nmemb, size_t size, struct general *general)
{
    void *res = calloc(nmemb, size);

    if (res == NULL)
        return NULL;
    add_free_list(general->free_list, res);

    return res;
}

void free_general(struct general *general)
{
    if (!general)
        return;

    my_free(general->prev_path, general);
    my_free(general->path, general);
    // free(general->prev_path);
    // free(general->path);
    // free(general->free_list);

    free(general);
    general = NULL;
}

/* ========================================================================== **
** -------------------------------- Free Ast -------------------------------- **
** ========================================================================== */

// void free_ast(struct node *node);
void free_ast(void *node_v, struct general *general);

static int free_if(struct node *node_basic, struct general *general)
{
    struct node_if *node = node_basic->structure;

    free_ast(node->condition, general);
    free_ast(node->node_then, general);
    free_ast(node->node_else, general);

    my_free(node, general);

    return 0;
}

static int free_command(struct node *node_basic, struct general *general)
{
    struct node_command *node = node_basic->structure;

    my_free(node->command, general);
    my_free(node, general);

    return 0;
}

static int free_while(struct node *node_basic, struct general *general)
{
    struct node_while *node = node_basic->structure;

    free_ast(node->condition, general);
    free_ast(node->node_then, general);

    my_free(node, general);

    return 0;
}

static int free_not(struct node *node_basic, struct general *general)
{
    struct node_not *node = node_basic->structure;

    free_ast(node->child, general);

    my_free(node, general);

    return 0;
}

// void free_ast(struct node *node)
void free_ast(void *node_v, struct general *general)
{
    struct node *node = node_v;

    struct free_function free_f[5] = {
        { .type = NODE_IF, .fun = free_if },
        { .type = NODE_COMMAND, .fun = free_command },
        { .type = NODE_WHILE, .fun = free_while },
        { .type = NODE_NOT, .fun = free_not },
        { .type = NODE_UNTIL, .fun = free_while },
    };

    struct node *root = node;

    struct node *next = node;

    while (next != NULL)
    {
        next = node->next;
        for (size_t i = 0; i < sizeof(free_f) / sizeof(*free_f);
             i++) // NB_NODE_TYPE; i++)
        {
            if (next && next->type == NODE_ROOT)
                free_ast(next, general);

            if (node->type == free_f[i].type)
            {
                free_f[i].fun(node, general);
                my_free(node, general);
                break;
            }
        }
        node = next;
    }

    my_free(root, general);
}

/*
int main()
{
    struct general *general = calloc(1, sizeof(struct general));

    general->free_list = free_list_init();

    char *str = my_malloc(20, general);
    char *str1 = my_malloc(20, general);
    char *str2 = my_malloc(20, general);
    char *str3 = my_malloc(20, general);
    char *str4 = my_calloc(20, 1, general);
    char *str5 = my_calloc(20, 1, general);


    my_free(str3, general);

    strcpy(str, "salut matthias");

    printf("%s\n", str);

    print_list(general->free_list);

    destroy(general->free_list);
    free_general(general);

    return 0;
}
*/
