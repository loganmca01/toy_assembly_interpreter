#  include <stdio.h>

struct command {
    char *name;
    struct ast_list *actions;
    struct sym_list *args;
};

struct symbol {
    double value;
    char *name;
};

struct sym_list {
    struct symbol *sym;
    struct sym_list *next;
};

#define NHASH 9997
extern struct command commandtab[NHASH];

/*
 * TODO: ADD WAY TO SPECIFY NUMBER OF GENERAL PURPOSE REGISTERS,
 *       AND SPECIAL REGISTERS, THEN MAKE EXTERN VAR INSTEAD OF CONSTANT.
 *       ADD 26 * 2 FOR DUMMY REGISTERS AND VARS IN ARGUMENTS.
 *       POPULATE IN SAME ORDER SO RETRIEVAL IS O(1).
 */


#define NUM_SYM 62
extern struct symbol symtab[NUM_SYM];

struct command *lookup(char*);

struct ast_list {
    struct ast *a;
    struct ast_list *next;
};

// To be implemented, will eventually let user dictate more details in virtual environment

struct system_information {
    int num_reg;
    int stack_start;
    int stack_size;
    int heap_start;
    int heap_size;
};

struct sym_list *new_sym_list(struct symbol *sym, struct sym_list *next);
struct ast_list *new_ast_list(struct ast *a, struct ast_list *next);

struct ast_list *add_ast(struct ast_list *orig, struct ast_list *addit);

// add - adds command to symbol table
// get - returns pointer to command from table
void add_command(char *name, struct ast_list *actions, struct sym_list *args);
struct command *get_command(char *name);

// might not be necessary, these would probably only be called
// when user is exiting simulation
void sym_list_free(struct sym_list *sl);
void ast_list_free(struct ast_list *astl);
void sym_table_free();
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
 * r        - general purpose register reference
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
    double number;
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
};

// nodetype m
struct memref {
    char nodetype;
    struct ast *loc;
};

// construct ast nodes, always cast back to ast pointer
struct ast *newast(char nodetype, struct ast *l, struct ast *r);
struct ast *newnum(double d);
struct ast *newcmp(char cmptype, struct ast *l, struct ast *r);
struct ast *newsymref(char reftype, struct symbol *s);
struct ast *newmemref(char nodetype, struct ast *loc);
struct ast *newflow(struct ast *cond, struct ast *then);

void dumpast(struct ast *a, int level);
int verify_ast(struct ast *a, struct sym_list *sl);
int verify_name(char *n, struct sym_list *sl);

// from flex, bison
extern FILE *yyin;
extern int yylineno;
extern int command_no;
void yyerror(char* s, ...);
