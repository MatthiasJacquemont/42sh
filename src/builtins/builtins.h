#ifndef BUILTINS_H
#define BUILTINS_H

#include "../general.h"

int echo(char *str, struct general *general);
int builtin_exit(char *str, struct general *general);
int cd(char *path, struct general *general, int modify);

#endif /* !BUILTINS_H */
