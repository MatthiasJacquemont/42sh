#ifndef VAR_H
#define VAR_H

#include "general.h"

void create_var(char *string, struct general *general);
void print_var(struct general *general);
int get_variable(char *string, struct general *general);
int contains_var(char *string);
int is_variable(char *string);
void add_to_var_list(char *value, char *name, struct general *general);

#endif /* !VAR_H */
