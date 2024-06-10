#include "ast_header.h"
#include <stdio.h>
#include <stdlib.h>



struct ast_list *new_ast_list(struct ast *a, struct ast_list *next) {

    struct ast_list *astl = malloc(sizeof(struct ast_list));

    if(!astl) {
        return NULL;
    }

    astl->a = a;
    astl->next = next;
    return astl;

}

struct ast_list *add_ast(struct ast_list *orig, struct ast_list *addit) {
    struct ast_list *mask = orig;
    while(mask->next) {
        mask = mask->next;
    }

    mask->next = addit;
    return orig;
}

struct sym_list *new_sym_list(struct symbol *sym, struct sym_list *next) {

    struct sym_list *sl = malloc(sizeof(struct sym_list));

    if(!sl) {
        return NULL;
    }

    sl->sym = sym;
    sl->next = next;
    return sl;

}

struct sym_list *add_sym(struct sym_list *orig, struct sym_list *addit) {

    struct sym_list *mask = orig;
    while(mask->next) {
        mask = mask->next;
    }

    mask->next = addit;
    return orig;

}

struct symbol *newsym(char *name, int type) {

    struct symbol *s = malloc(sizeof(struct symbol));

    s->name = name;
    s->type = type;

    return s;

}



struct ast *newast(char nodetype, struct ast *l, struct ast *r)
{
    struct ast *a = malloc(sizeof(struct ast));

    if(!a) {
        return NULL;
    }
    a->nodetype = nodetype;
    a->l = l;
    a->r = r;
    return a;
}

struct ast *newnum(int d)
{
    struct numval *a = malloc(sizeof(struct numval));

    if(!a) {
        return NULL;
    }
    a->nodetype = 'n';
    a->number = d;
    return (struct ast *)a;
}

struct ast *newcmp(char cmptype, struct ast *l, struct ast *r)
{
    struct ast *a = malloc(sizeof(struct ast));

    if(!a) {
        return NULL;
    }
    a->nodetype = cmptype;
    a->l = l;
    a->r = r;
    return a;
}

struct ast *newsymref(char reftype, struct symbol *s, char *name) {

    struct symref *a = malloc(sizeof(struct symref));

    if (!a) {
        return NULL;
    }

    a->nodetype = reftype;
    a->sym = s;
    a->name = name;
    return (struct ast *)a;

}

struct ast *newmemref(char nodetype, struct ast *loc) {

    struct memref *a = malloc(sizeof(struct memref));

    if (!a) {
        return NULL;
    }

    a->nodetype = nodetype;
    a->loc = loc;
    return (struct ast *)a;

}

struct ast *newflow(struct ast *cond, struct ast *then) {

    struct flow *a = malloc(sizeof(struct flow));

    if (!a) {
        return NULL;
    }

    a->nodetype = 'i';
    a->cond = cond;
    a->then = then;
    return (struct ast *)a;

}

void sym_list_free(struct sym_list *sl)
{
    struct sym_list *nsl;

    while(sl) {
        nsl = sl->next;
        free(sl);
        sl = nsl;
    }
}

void ast_list_free(struct ast_list *astl) {

    struct ast_list *nastl;

    while(astl) {
        nastl = astl->next;
        treefree(astl->a);
        free(astl);
        astl = nastl;
    }

}


void treefree(struct ast *a) {

    switch(a->nodetype) {

        /* two subtrees */
        case '+':
        case '-':
        case '*':
        case '/':
        case '=':
        case '1':  case '2':  case '3':  case '4':  case '5':
            treefree(a->r);
            treefree(a->l);

        /* no subtree */
        case 'n': case 'r': case 'v':
            break;

        case 'i':
            free( ((struct flow *)a)->cond);
            free( ((struct flow *)a)->then);
            break;

        case 'm':
            free(((struct memref *) a)->loc);
            break;

        default: printf("internal error: free bad node %c\n", a->nodetype);
    }

    // free the node itself
    free(a);

}
