#include "vm.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

int *registers;
uint8_t *memory;

struct system_information sys_info;
char *arena;
int arena_pointer;

struct command instructions[OPCODE_MAX];

struct ast **ast_stack;
int ast_stack_ptr;

struct ast *pop_stack() {
    return ast_stack[ast_stack_ptr--];
}

void push_stack(struct ast *node) {
    ast_stack_ptr++;
    ast_stack[ast_stack_ptr] = node;
}

void yyerror(char *s, ...) {
    va_list ap;
    va_start(ap, s);

    fprintf(stderr, "line %d: error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");

}


/* for testing parser */
void print_system_info() {
    
    printf("num instructions: %d\n", sys_info.num_instructions);

    printf("num regs: %d\n", sys_info.num_regs);

    for (int i = 0; i < sys_info.num_regs; i++) {
        printf("%d: ", i);
        if (sys_info.reg_names[i]) printf("%s", sys_info.reg_names[i]);
        printf("\n");
    }

    printf("mem size: %d\n", sys_info.mem_size);

    printf("num mem regions: %d\n", sys_info.num_regions);

    for (int i = 0; i < sys_info.num_regions; i++) {
        printf("%s %d %d %d\n", sys_info.mem_regions[i].name, sys_info.mem_regions[i].base, sys_info.mem_regions[i].bound, sys_info.mem_regions[i].direction);
    }

}

void *arena_allocate(size_t size) {
    void *start = &arena[arena_pointer];
    arena_pointer += size;
    if (arena_pointer > ARENA_SIZE) return NULL;
    return start;
}


int main(int argc, char **argv) {

    arena = malloc(sizeof (char) * ARENA_SIZE);
    arena_pointer = 0;

    char buff[64];

    strcpy(buff, "../");
    strcat(buff, argv[1]);

    yyin = fopen(argv[1], "r");

    if (yyin == NULL) {
        fprintf(stderr, "error opening instruction set file\n");
        exit(1);
    }

    yyparse();

    fclose(yyin);

    print_system_info();

    printf("%d\n", arena_pointer);

}

/* below are redefinitions of ast struct creator functions using arena_allocate instead of malloc */

struct ast_list *new_ast_list_a(struct ast *a, struct ast_list *next) {

    struct ast_list *astl = arena_allocate(sizeof(struct ast_list));

    if(!astl) {
        return NULL;
    }

    astl->a = a;
    astl->next = next;
    return astl;

}

struct sym_list *new_sym_list_a(struct symbol *sym, struct sym_list *next) {

    struct sym_list *sl = arena_allocate(sizeof(struct sym_list));

    if(!sl) {
        return NULL;
    }

    sl->sym = sym;
    sl->next = next;
    return sl;

}


struct symbol *newsym_a(char *name, int type, int value) {

    struct symbol *s = arena_allocate(sizeof(struct symbol));

    s->name = name;
    s->type = type;
    s->value = value;

    return s;

}



struct ast *newast_a(char nodetype, struct ast *l, struct ast *r)
{
    struct ast *a = arena_allocate(sizeof(struct ast));

    if(!a) {
        return NULL;
    }
    a->nodetype = nodetype;
    a->l = l;
    a->r = r;
    return a;
}

struct ast *newnum_a(int d)
{
    struct numval *a = arena_allocate(sizeof(struct numval));

    if(!a) {
        return NULL;
    }
    a->nodetype = 'n';
    a->number = d;
    return (struct ast *)a;
}

struct ast *newcmp_a(char cmptype, struct ast *l, struct ast *r)
{
    struct ast *a = arena_allocate(sizeof(struct ast));

    if(!a) {
        return NULL;
    }
    a->nodetype = cmptype;
    a->l = l;
    a->r = r;
    return a;
}

struct ast *newsymref_a(char reftype, struct symbol *s, char *name) {

    struct symref *a = arena_allocate(sizeof(struct symref));

    if (!a) {
        return NULL;
    }

    a->nodetype = reftype;
    a->sym = s;
    a->name = name;
    return (struct ast *)a;

}

struct ast *newmemref_a(char nodetype, struct ast *loc) {

    struct memref *a = arena_allocate(sizeof(struct memref));

    if (!a) {
        return NULL;
    }

    a->nodetype = nodetype;
    a->loc = loc;
    return (struct ast *)a;

}

struct ast *newflow_a(struct ast *cond, struct ast *then) {

    struct flow *a = arena_allocate(sizeof(struct flow));

    if (!a) {
        return NULL;
    }

    a->nodetype = 'i';
    a->cond = cond;
    a->then = then;
    return (struct ast *)a;

}
