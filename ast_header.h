


struct command {
    char *name;
    struct ast_list *actions;
    struct sym_list *args;
};

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

struct memory_region {

    char *name;

    int base;
    int bound;
    
    /* 0 for up, 1 for down */
    int direction;

};

struct system_information {
    int num_instructions;
    
    int num_regs;
    char **reg_names;

    int mem_size;
    int num_regions;
    struct memory_region *mem_regions;

    /* todo: add support for different memory regions
    int num_regions;
    struct memory_region *regions;
    */

};

struct sym_list *new_sym_list(struct symbol *sym, struct sym_list *next);
struct ast_list *new_ast_list(struct ast *a, struct ast_list *next);

struct sym_list *add_sym(struct sym_list *orig, struct sym_list *addit);
struct ast_list *add_ast(struct ast_list *orig, struct ast_list *addit);

// might not be necessary, these would probably only be called
// when user is exiting simulation
void sym_list_free(struct sym_list *sl);
void ast_list_free(struct ast_list *astl);
void reg_table_free();
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
 * r - register only
 * v - variable/register (default until assignment)
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
struct ast *newnum(int d);
struct ast *newcmp(char cmptype, struct ast *l, struct ast *r);
struct ast *newsymref(char reftype, struct symbol *s, char *name);
struct ast *newmemref(char nodetype, struct ast *loc);
struct ast *newflow(struct ast *cond, struct ast *then);