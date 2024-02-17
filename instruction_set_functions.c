//
// Created by logan on 2/3/2024.
//

#include "instruction_set.h"
#  include <stdlib.h>
#  include <stdarg.h>
#  include <string.h>
#  include <math.h>

struct command commandtab[NHASH];
struct symbol symtab[NUM_SYM];

// hash function for command, pretty simple
static int command_hash(char *name)
{
    int hash = 11;
    char c;

    while((c = *name++)) hash = hash * 17 + c;

    return hash % NHASH;
}

void add_command(char *name, struct ast_list *actions, struct sym_list *args) {

    struct command *c = &commandtab[command_hash(name)];

    int scount = 0;

    while(scount++ < NHASH) {

        /* command found */
        if(c->name && !strcmp(c->name, name)) {
            yyerror("command with name %s already defined\n", name);
        }

        /* new command */
        if(!c->name) {
            c->name = name;
            c->actions = actions;
            c->args = args;
            return;
        }

        if(++c >= symtab+NHASH) c = commandtab;
    }
    yyerror("command table overflow\n");

}

struct command *get_command(char *name) {
    struct command *c = &commandtab[command_hash(name)];

    int scount = 0;

    while(scount++ < NHASH) {
        /* command found */
        if(c->name && !strcmp(c->name, name)) {
            return c;
        }

        /* command doesn't exist */
        if(!c->name) {
            fprintf(stderr, "command not found\n");
            exit(1);
        }

        if(++c >= symtab+NHASH) c = commandtab;
    }
}

struct ast_list *new_ast_list(struct ast *a, struct ast_list *next) {

    struct ast_list *astl = malloc(sizeof(struct ast_list));

    if(!astl) {
        yyerror("out of space");
        exit(0);
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
        yyerror("out of space");
        exit(0);
    }

    sl->sym = sym;
    sl->next = next;
    return sl;

}


struct ast *newast(char nodetype, struct ast *l, struct ast *r)
{
    struct ast *a = malloc(sizeof(struct ast));

    if(!a) {
        yyerror("out of space");
        exit(0);
    }
    a->nodetype = nodetype;
    a->l = l;
    a->r = r;
    return a;
}

struct ast *newnum(double d)
{
    struct numval *a = malloc(sizeof(struct numval));

    if(!a) {
        yyerror("out of space");
        exit(0);
    }
    a->nodetype = 'n';
    a->number = d;
    return (struct ast *)a;
}

struct ast *newcmp(char cmptype, struct ast *l, struct ast *r)
{
    struct ast *a = malloc(sizeof(struct ast));

    if(!a) {
        yyerror("out of space");
        exit(0);
    }
    a->nodetype = cmptype;
    a->l = l;
    a->r = r;
    return a;
}

struct ast *newsymref(char reftype, struct symbol *s) {

    struct symref *a = malloc(sizeof(struct symref));

    if (!a) {
        yyerror("out of space");
        exit(0);
    }

    a->nodetype = reftype;
    a->sym = s;
    return (struct ast *)a;

}

struct ast *newmemref(char nodetype, struct ast *loc) {

    struct memref *a = malloc(sizeof(struct memref));

    if (!a) {
        yyerror("out of space");
        exit(0);
    }

    a->nodetype = nodetype;
    a->loc = loc;
    return (struct ast *)a;

}

struct ast *newflow(struct ast *cond, struct ast *then) {

    struct flow *a = malloc(sizeof(struct flow));

    if (!a) {
        yyerror("out of space");
        exit(0);
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

void sym_table_free() {

    for (int i = 0; i < NUM_SYM; i++) {
        free(symtab[i].name);
    }
}

void command_table_free() {

    for (int i = 0; i < NHASH; i++) {
        // TODO: test if this actually works as condition
        if (commandtab[i].name) {
            free(commandtab[i].name);
            ast_list_free(commandtab[i].actions);
            sym_list_free(commandtab[i].args);
        }
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

void dumpast(struct ast *a, int level) {

    printf("%*s", 2*level, "");	/* indent to this level */
    level++;

    if(!a) {
        printf("NULL\n");
        return;
    }

    switch(a->nodetype) {
        /* constant */
        case 'n':
            printf("number %4.4g\n", ((struct numval *)a)->number);
            break;

            /* expressions, comparisons, assignment*/
        case '+': case '-': case '*': case '/': case '=':
        case '1': case '2': case '3': case '4': case '5':
            printf("operation %c\n", a->nodetype);
            dumpast(a->l, level);
            dumpast(a->r, level);
            return;

            /* if statement, dump conditional and result */
        case 'i':
            printf("flow %c\n", a->nodetype);
            dumpast( ((struct flow *)a)->cond, level);
            dumpast( ((struct flow *)a)->then, level);
            return;

            /* variable, register, and memory asts */
        case 'v':
            printf("var ref %s\n", ((struct symref *)a)->sym->name);
            return;
        case 'r':
            printf("reg ref %s\n", ((struct symref *)a)->sym->name);
            return;
        case 'm':
            printf("mem ref\n");
            dumpast(((struct memref *)a)->loc, level);
            // TODO: maybe another print here instead? test later
            return;

        default:
            printf("bad %c\n", a->nodetype);
            return;
    }
}

// helper function, generates default register and variable symbols
// will eventually be made less strict, less arbitrary
void generate_symbols() {

    for (int i = 0; i < 26; i++) {
        symtab[i].name = strdup("reg0");
        symtab[i].name[3] = 97  + i;
    }

    for (int i = 26; i < 52; i++) {
        symtab[i].name = strdup("var0");
        symtab[i].name[3] = 71  + i;
    }

    symtab[52].name = strdup("AC");
    symtab[53].name = strdup("PC");
    symtab[54].name = strdup("SP");
    symtab[55].name = strdup("BP");

    for (int i = 56; i < NUM_SYM; i++) {
        symtab[i].name = strdup("regx");
        symtab[i].name[3] = i - 8;
    }

}

void yyerror(char *s, ...) {
    va_list ap;
    va_start(ap, s);

    fprintf(stderr, "%d: error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");

}

int main(int argc, char **argv) {
    /* Process command line args*/
    yyin = fopen(argv[1], "r");

    /* generate default register and variable symbols */
    generate_symbols();

    yyparse();

    fclose(yyin);

    char *com_name = argv[2];

    struct command *c = get_command(com_name);

    struct ast_list *ca = c->actions;

    while (ca) {
        dumpast(ca->a, 0);
        ca = ca->next;
    }

    return 0;
}
