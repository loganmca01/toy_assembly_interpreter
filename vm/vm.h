#include "../ast_header.h"
#include <stdio.h>

/* system globals */
extern struct symbol *registers;
extern struct symbol PC;
extern uint8_t *memory;
extern struct system_information sys_info;

#define OPCODE_MAX (256)
extern struct command instructions[OPCODE_MAX];

#define ARENA_SIZE (32768)
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

/* ast stack for rebuilding asts from isa file */
#define STACK_MAX (64)

extern struct ast **ast_stack;
extern int ast_stack_ptr;

struct ast *pop_stack();
void push_stack(struct ast *node);

extern struct sym_list *current_sl;

/* handle node takes attributes to help function know if it needs to check a reference or push next node to stack */
#define IS_REF (0b1)
#define IS_PUSHED (0b10)
int handle_node(struct ast *current, struct ast *next, int attributes);

/* Flex and Bison functions/vars */
extern FILE *yyin;
extern int yylineno;
extern int yyparse(void);
void yyerror(char* s, ...);

/* ipc functions and structs */

#define IN_MAX 256
#define OUT_MAX 1024

struct in_msgbuf {
    long mtype;
    char mtext[IN_MAX];
};

struct out_msgbuf {
    long mtype;
    char mtext[OUT_MAX];
};

int open_connection();
int send_message(char *data);
int receive_message(struct in_msgbuf *in);

/* VM functions */
int handle_command(char *command);
int run_load(char *filename);