#include <stdio.h>
#include <stdint.h>

struct command {
    char *name;
    struct ast_list *actions;
    struct sym_list *args;
};

/*
 * symbol types
 * 0 - register
 * 1 - variable
 */
struct symbol {
    int value;
    int type;
    char *name;
};

struct sym_list {
    struct symbol *sym;
    struct sym_list *next;
};

struct sym_map {
    struct symbol *dummy;
    struct symbol *real;
};

#define NHASH 9997
extern struct command commandtab[NHASH];

/*
 * TODO: ADD WAY TO SPECIFY NUMBER OF GENERAL PURPOSE REGISTERS,
 *       AND SPECIAL REGISTERS, THEN MAKE EXTERN VAR INSTEAD OF CONSTANT.
 *       ADD 26 * 2 FOR DUMMY REGISTERS AND VARS IN ARGUMENTS.
 *       POPULATE IN SAME ORDER SO RETRIEVAL IS O(1).
 */


/*
 * TODO CONSIDER: should this symbol table be a hash table instead? and get rid of the
 * dummy variables and registers and just let user use their own variable names?
 *
 * TODO CONSIDER: How do I differentiate between vars and registers if I do this?
 * TODO CONSIDER: What if I make symbol table only for special purpose registers and
 * make all other symbols just specific to commands? TODO !!!!!!!
 *
 */
extern unsigned int NUM_SPEC_REG;
extern unsigned int NUM_GEN_REG;
extern struct symbol *spec_reg;
extern struct symbol *gen_reg;

extern unsigned int STACK_SIZE;
extern unsigned int STACK_START;

extern unsigned int CODE_SIZE;
extern unsigned int CODE_START;

extern uint8_t *stack;
extern char **code;

struct ast_list {
    struct ast *a;
    struct ast_list *next;
};

// To be implemented, will eventually let user dictate more details in virtual environment

/*
struct system_information {
    int num_reg;
    int stack_start;
    int stack_size;
    int heap_start;
    int heap_size;
};
*/

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
struct ast *newsymref(char reftype, struct symbol *s);
struct ast *newmemref(char nodetype, struct ast *loc);
struct ast *newflow(struct ast *cond, struct ast *then);

int eval_cmp(struct ast *a);
int eval_ast(struct ast *a);
void dump_ast(struct ast *a, int level);
int verify_ast(struct ast *a, struct sym_list *sl);
int verify_name(char *n, struct sym_list *sl);

/*
 * User environment
 *
 * */

void run_user();

void print_welcome();
void print_spec_regs();
void print_help_message();
void print_system_info();
void print_bin(char byte_val);

/* helper functions for running commands */
int run_instruction(char *instr);
int run_action(struct sym_list *sl, struct ast *a, struct sym_map *reg_map);

int run_dot(char *input);
void run_print(int type, char *args);
void run_clear(int type, char *args);


// from flex, bison
extern FILE *yyin;
extern int yylineno;
extern int command_no;
void yyerror(char* s, ...);
