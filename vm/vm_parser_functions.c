#include "vm.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>


struct ast **ast_stack;
int ast_stack_ptr;

struct sym_list *current_sl;

struct ast *pop_stack() {
    if (ast_stack_ptr < 0) return NULL;
    return ast_stack[ast_stack_ptr--];
}

void push_stack(struct ast *node) {
    ast_stack_ptr++;
    ast_stack[ast_stack_ptr] = node;
}

int handle_node(struct ast *current, struct ast *next, int attributes) {
    
    /* if the next node is a reference, check that it's  */
    if (attributes & IS_REF) {
        struct symref *n = (struct symref *) next;
        if (!strcmp(n->name, "PC")) {
            n->sym = &PC;
            goto end_ref;
        }
        for (int i = 0; i < sys_info.num_regs; i++) {
            if (sys_info.reg_names[i] && !strcmp(sys_info.reg_names[i], n->name)) {
                n->sym = &registers[i];
                goto end_ref;
            }
        }
        for (struct sym_list *sl = current_sl; sl; sl = sl->next) {
            if (!strcmp(sl->sym->name, n->name)) {
                n->sym = sl->sym;
                goto end_ref;
            }
        }
        return 1;
    }

    /* goto jump point for when reference is found. cleanest way to escape whole section */
    end_ref:;
    
    if (current == NULL) {
        if (attributes & IS_PUSHED) push_stack(next);
        return 0;
    }
    else {
        switch (current->nodetype) {
            case '=': case '+': case '-': case '*': case '/': case '|': case '&': case '^':
            case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8':
                if (current->l == NULL) {
                    current->l = next;
                    push_stack(current);
                    if (attributes & IS_PUSHED) push_stack(next);
                    return 0;
                }
                else if (current->r == NULL) {
                    current->r = next;
                    if (attributes & IS_PUSHED) push_stack(next);
                    return 0;
                }
                else {
                    return 1;
                }
            case 'i':
                struct flow *f = (struct flow *) current;
                if (f->cond == NULL) {
                    f->cond = next;
                    push_stack(current);
                    if (attributes & IS_PUSHED) push_stack(next);
                    return 0;
                }
                else if (f->then == NULL) {
                    f->then = next;
                    if (attributes & IS_PUSHED) push_stack(next);
                    return 0;
                }
                else {
                    return 1;
                }
            case 'm':
                struct memref *m = (struct memref *) current;
                if (m->loc == NULL) {
                    m->loc = next;
                    if (attributes & IS_PUSHED) push_stack(next);
                    return 0;
                }
                else {
                    return 1;
                }
            default:
                return 1;


        }
    }
}

void yyerror(char *s, ...) {
    va_list ap;
    va_start(ap, s);

    fprintf(stderr, "line %d: error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");

}


void *arena_allocate(size_t size) {
    void *start = &arena[arena_pointer];
    arena_pointer += size;
    if (arena_pointer > ARENA_SIZE) return NULL;
    return start;
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
