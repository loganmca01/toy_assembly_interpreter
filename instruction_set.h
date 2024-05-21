#include <stdio.h>
#include <stdint.h>

struct command {
    char *name;
    struct ast_list *actions;
    struct sym_list *args;
};

#define MAX_COMMAND 1024


/*
 * symbol types
 * 0 - register
 * 1 - variable
 *
 * todo: remove value from symbols, and fix all bugs that creates. no longer needed since user env elsewhere
 */
struct symbol {
    char *name;
    int type;
};

struct sym_list {
    struct symbol *sym;
    struct sym_list *next;
};

struct ast_list {
    struct ast *a;
    struct ast_list *next;
};

// To be implemented, will eventually let user dictate more details in virtual environment


struct system_information {
    int num_spec_reg;
    int num_gen_reg;
    char **spec_regs;
    char **gen_regs;

    int stack_start;
    int stack_size;
    int code_start;
    int code_size;

    char lit_sym;
    char reg_sym;
};

extern struct system_information sys_info;

void generate_default_system();

struct sym_list *new_sym_list(struct symbol *sym, struct sym_list *next);
struct ast_list *new_ast_list(struct ast *a, struct ast_list *next);

struct sym_list *add_sym(struct sym_list *orig, struct sym_list *addit);
struct ast_list *add_ast(struct ast_list *orig, struct ast_list *addit);

// add - adds command to symbol table
// get - returns pointer to command from table
void add_command(char *name, struct ast_list *actions, struct sym_list *args);

// might not be necessary, these would probably only be called
// when user is exiting simulation
void sym_list_free(struct sym_list *sl);
void ast_list_free(struct ast_list *astl);
void reg_tables_free();
void command_table_free();
void treefree(struct ast *);


/**
 * Node types in ast
 *
 * + - * /  - operations
 * 1-5      - comparison ops
 * i        - conditional
 * =        - assignment
 * v        - variable reference
 * r        - register reference
 * n        - number
 * m        - memory location
 *
 */
struct ast {
    char nodetype;
    struct ast *l;
    struct ast *r;
};

// nodetype i
struct flow {
    char nodetype;
    struct ast *cond;
    struct ast *then;
};

// nodetype n
struct numval {
    char nodetype;
    int number;
};

/**
 * 2 different types of symbol reference
 *
 * r - register
 * v - variable
 *
 */
struct symref {
    char nodetype;
    struct symbol *sym;
    char *name;
};

// nodetype m
struct memref {
    char nodetype;
    struct ast *loc;
};

struct symbol *newsym(char *name, int type);

// construct ast nodes, always cast back to ast pointer
struct ast *newast(char nodetype, struct ast *l, struct ast *r);
struct ast *newnum(double d);
struct ast *newcmp(char cmptype, struct ast *l, struct ast *r);
struct ast *newsymref(char reftype, struct symbol *s, char *name);
struct ast *newmemref(char nodetype, struct ast *loc);
struct ast *newflow(struct ast *cond, struct ast *then);

void dump_ast(FILE *f, struct ast *a);
int verify_ast(struct ast *a, struct sym_list *sl);
int verify_ref(struct symref *symr, struct sym_list *sl);


// from flex, bison
extern FILE *yyin;
extern int yylineno;
extern int command_no;
void yyerror(char* s, ...);
