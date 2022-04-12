#ifndef FREE_H
#define FREE_H

#include <stddef.h>

#include "general.h"
#include "lexer/lexer.h"
#include "new_parser/ast.h"

struct free_list *free_list_init(void);
void my_free(void *ptr, struct general *general);
void destroy(struct free_list *free_list);
void *my_malloc(size_t size, struct general *general);
void *my_calloc(size_t nmemb, size_t size, struct general *general);

void free_general(struct general *general);
// void free_ast(struct node *node);
void free_ast(void *node, struct general *general);

#endif /* !FREE_H */
