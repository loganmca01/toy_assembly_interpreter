#include "../ast_header.h"
#include <stdio.h>

/* system globals */
extern int *registers;
extern uint8_t *memory;
extern struct system_information sys_info;

#define OPCODE_MAX 256
extern struct command instructions[OPCODE_MAX];

#define ARENA_SIZE 8192
extern char *arena;
extern int arena_pointer;

/* arena allocator */
void *arena_allocate(size_t size);

/* redefinitions of "new" functions from ast header using arena_allocate instead of malloc 
*  NOTE: functions are exactly identical otherwise, not sure if there's a cleaner way to do this
*/
struct sym_list *new_sym_list_a(struct symbol *sym, struct sym_list *next);
struct ast_list *new_ast_list_a(struct ast *a, struct ast_list *next);
struct symbol *newsym_a(char *name, int type, int value);
struct ast *newast_a(char nodetype, struct ast *l, struct ast *r);
struct ast *newnum_a(int d);
struct ast *newcmp_a(char cmptype, struct ast *l, struct ast *r);
struct ast *newsymref_a(char reftype, struct symbol *s, char *name);
struct ast *newmemref_a(char nodetype, struct ast *loc);
struct ast *newflow_a(struct ast *cond, struct ast *then);


/* Flex and Bison functions/vars */
extern FILE *yyin;
extern int yylineno;
extern int yyparse(void);
void yyerror(char* s, ...);

/* VM functions */