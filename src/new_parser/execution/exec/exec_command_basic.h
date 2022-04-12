#ifndef EXEC_COMMAND_H
#define EXEC_COMMAND_H

#include "../../ast.h"

int exec_command(struct node *node, struct general *general);
int exec_builtin(struct node *node, struct general *general);
int exec_if(struct node *node, struct general *general);
int exec_while(struct node *node, struct general *general);
int exec_until(struct node *node, struct general *general);
int exec_not(struct node *node, struct general *general);
int exec_and(struct node *node, struct general *general);
int exec_or(struct node *node, struct general *general);

#endif /* !EXEC_COMMAND_H */
