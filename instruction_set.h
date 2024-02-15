#  include <stdio.h>

struct command {
    char *name;
    struct ast_list *actions;
    struct sym_list *args;
};

struct symbol {
    char *name;
    double value;
};

struct sym_list {
    struct symbol *sym;
    struct sym_list *next;
};

#define NHASH 9997
extern struct command commandtab[NHASH];

/*
 * TODO: ADD WAY TO SPECIFY NUMBER OF GENERAL PURPOSE REGISTERS,
 *       AND SPECIAL REGISTERS, THEN MAKE EXTERN VAR INSTEAD OF MACRO.
 *       ADD 26 * 2 FOR DUMMY REGISTERS AND VARS IN ARGUMENTS.
 *       POPULATE IN SAME ORDER SO RETRIEVAL IS O(1).
 */

// NOTE: ONLY DUMMY VARIABLE AND REGISTER VALUES WILL ACTUALLY CHANGE
// FROM PARAMETERS IN FUNCTION CALL

#define NUM_SYM 62
extern struct symbol symtab[NUM_SYM];

struct command *lookup(char*);


struct ast_list {
    struct ast *a;
    struct ast_list *next;
};

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
 *
 *          NOTE: hacky solution for offset without having to add to struct for sign.
 * M        - memory address with positive offset
 * m        - memory address with negative offset
 *
 */
struct ast {
    char nodetype;
    struct ast *l;
    struct ast *r;
};

struct flow {
    char nodetype;
    struct ast *cond;
    struct ast *then;
};

// node type n
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

struct memref {
    char nodetype;
    struct ast *loc; // points to another ast node, either symref, numval, or operation
    struct ast *offset;
};

struct ast *newast(char nodetype, struct ast *l, struct ast *r);
struct ast *newnum(double d);
struct ast *newcmp(char cmptype, struct ast *l, struct ast *r);
struct ast *newsymref(char reftype, struct symbol *s);
struct ast *newmemref(char nodetype, struct ast *loc, struct ast *offset);
struct ast *newflow(struct ast *cond, struct ast *then);

void dumpast(struct ast *a, int level);

extern FILE *yyin;
extern int yylineno;
void yyerror(char* s, ...);
