#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "../../builtins/builtins.h"
#include "../ast.h"
#include "exec/exec_command_basic.h"

// #define TYPE_SIZE 5

int execution(struct node *root, struct general *general)
{
    if (!general)
        return 2;

    struct function funs[7] = {
        { .type = NODE_IF, .fun = exec_if },
        { .type = NODE_COMMAND, .fun = exec_command },
        { .type = NODE_WHILE, .fun = exec_while },
        { .type = NODE_NOT, .fun = exec_not },
        { .type = NODE_AND, .fun = exec_and },
        { .type = NODE_OR, .fun = exec_or },
        { .type = NODE_UNTIL, .fun = exec_until },
    };

    int res = 0;

    struct node *node = root;

    for (; node != NULL; node = node->next)
    {
        cd(general->path, general, 0); // 0 to not modify prev path
        for (size_t i = 0; i < sizeof(funs) / sizeof(*funs); i++)
        {
            if (node->type == funs[i].type)
            {
                res = funs[i].fun(node, general);
                break;
            }
        }
    }

    return res;
}

/*
int main()
{
    struct node *node = malloc(sizeof(struct node));
    struct node *node_c = malloc(sizeof(struct node));
    struct node *node_t = malloc(sizeof(struct node));
    struct node *node_e = malloc(sizeof(struct node));

    struct node_command *nodec_c = malloc(sizeof(struct node_command));
    struct node_command *nodec_t = malloc(sizeof(struct node_command));
    struct node_command *nodec_e = malloc(sizeof(struct node_command));




    struct node_if *command = malloc(sizeof(struct node_command));


    char *str = calloc(1, 100);
    char *str1 = calloc(1, 100);
    char *str2 = calloc(1, 100);

    strcpy(str, "eco Salut je suis laefce sd");
    strcpy(str1, "echo C'est vrai");
    strcpy(str2, "echo C'est faux");


    nodec_c->command = str;
    nodec_t->command = str1;
    nodec_e->command = str2;



    node->type = NODE_IF;

    node_c->type = NODE_COMMAND;
    node_t->type = NODE_COMMAND;
    node_e->type = NODE_COMMAND;




    node_c->structure = nodec_c;
    node_t->structure = nodec_t;
    node_e->structure = nodec_e;

    command->condition = node_c;
    command->node_then = node_t;
    command->node_else = node_e;


    node->structure = command;

    execution(node);

    return 1;
}
*/
