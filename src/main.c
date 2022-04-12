#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "free_ast.h"
#include "lexer/lexer.h"
#include "new_parser/ast.h"
#include "new_parser/execution/ast_execution.h"
#include "variable.h"

// #define NB_NODE_TYPE 2

/*
struct list *free_list;

void list_free(struct list *list)
{
    while (list)
        free_list(list->next);

    free(list);
}
*/

// void free_ast(struct node *node);

int main(int argc, char *argv[])
{
    int call = -1;
    int res = 2;

    struct general *general = calloc(1, sizeof(struct general));
    general->free_list = free_list_init();

    while (call == -1)
    {
        char *string = NULL;
        call = args_to_string(argc, argv, &string, general);

        if (call == 18)
        {
            destroy(general->free_list);
            free(general);
            return 0;
        }

        if (argc == 1)
        {
            int i = 0;
            int nb_newline = 0;

            while (string[i] != '\0')
            {
                if (string[i++] == '\n')
                    nb_newline++;
            }

            if (nb_newline == 1)
                string[strlen(string) - 1] = '\0';
        }

        if (call == 1)
        {
            destroy(general->free_list);
            // free_general(general);
            free(general);
            return 127;
        }

        if (string && strlen(string) == 0)
        {
            destroy(general->free_list);
            // free_general(general);
            free(general);
            return res;
        }

        /*
                int index = 0;
                int jump;

                while (string[index] != '\0')
                {
                    jump = get_variable(string + index, general);
                    if (string[index + jump] == '\0')
                        break;
                    index += jump + 1;
                }
                */

        struct node *ast = parser(string, general);

        char *path = my_calloc(4, 1, general);
        char *prev_path = my_calloc(4, 1, general);

        strcpy(path, "./");
        strcpy(prev_path, "./");

        general->path = path;
        general->prev_path = prev_path;

        if (call == 2)
        {
            // free_ast(ast, general);
            destroy(general->free_list);
            // free_general(general);
            free(general);
            return 0;
        }

        if (!ast && call != -1)
        {
            // free_ast(ast, general);
            destroy(general->free_list);
            // free_general(general);
            free(general);
            exit(2);
        }

        res = execution(ast, general);
        if (res == 255)
            res = 127;

        if (res > 1)
            warnx("Error during execution");

        // list_free(free_list);
        // free_ast(ast, general);
    }
    destroy(general->free_list);
    // free_general(general);
    free(general);

    // printf("res : %d\n", res);
    return res;
}

/*
int free_if(struct node *node_basic)
{
    // (void)node_basic->next;

    struct node_if *node = node_basic->structure;

    free_ast(node->condition);
    free_ast(node->node_then);
    free_ast(node->node_else);

    free(node);

    return 0;
}

int free_command(struct node *node_basic)
{
    struct node_command *node = node_basic->structure;

    free(node->command);
    free(node);

    return 0;
}

void free_ast(struct node *node)
{
    struct function free_f[NB_NODE_TYPE] = {
        { .type = NODE_IF, .fun = free_if },
        { .type = NODE_COMMAND, .fun = free_command },
    };

    struct node *root = node;

    struct node *next = node;

    while (next != NULL)
    {
        next = node->next;
        for (size_t i = 0; i < NB_NODE_TYPE; i++)
        {
            if  (node->type == free_f[i].type)
            {
                free_f[i].fun(node);
                free(node);
                break;
            }
        }
        node = next;
    }

    free(root);
}
*/
