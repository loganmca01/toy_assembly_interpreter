//
// Created by logan on 2/3/2024.
//

#include "instruction_set.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

struct command commandtab[NHASH];
struct symbol *spec_reg;
struct symbol *gen_reg;

unsigned int NUM_SPEC_REG;
unsigned int NUM_GEN_REG;

// hash function for command, pretty simple
static unsigned int command_hash(char *name)
{
    unsigned int hash = 11;
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

        if(++c >= commandtab+NHASH) c = commandtab;
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
            return NULL;
        }

        if(++c >= commandtab+NHASH) c = commandtab;
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

struct sym_list *add_sym(struct sym_list *orig, struct sym_list *addit) {

    struct sym_list *mask = orig;
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

struct symbol *newsym(char *name, int type) {

    struct symbol *s = malloc(sizeof(struct symbol));

    s->name = name;
    s->type = type;
    s->value = 0;

    return s;

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

struct ast *newsymref(char reftype, struct symbol *s, char *name) {

    struct symref *a = malloc(sizeof(struct symref));

    if (!a) {
        yyerror("out of space");
        exit(0);
    }

    a->nodetype = reftype;
    a->sym = s;
    a->name = name;
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

void reg_tables_free() {

    for (int i = 0; i < NUM_SPEC_REG; i++) {
        free(spec_reg[i].name);
    }
    for (int i = 0; i < NUM_GEN_REG; i++) {
        free(spec_reg[i].name);
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

int eval_cmp(struct ast *a) {
    if(!a) {
        fprintf(stderr, "missing cond node\n");
        return 0;
    }

    switch(a->nodetype) {

        case '1':
            return eval_ast(a->r) > eval_ast(a->l);
        case '2':
            return eval_ast(a->r) < eval_ast(a->l);
        case '3':
            return eval_ast(a->r) == eval_ast(a->l);
        case '4':
            return eval_ast(a->r) >= eval_ast(a->l);
        case '5':
            return eval_ast(a->r) <= eval_ast(a->l);
        default:
            fprintf(stderr, "error: invalid comparison type\n");
            return 0;

    }

}

int eval_ast(struct ast *a) {

    if(!a) {
        fprintf(stderr, "missing ast node\n");
        return 0;
    }

    switch(a->nodetype) {
            /* constant */
        case 'n':
            return (((struct numval *)a)->number);

            /* expressions */
        case '+':
            return eval_ast(a->l) + eval_ast(a->r);
        case '-':
            return eval_ast(a->l) - eval_ast(a->r);
        case '*':
            return eval_ast(a->l) * eval_ast(a->r);
        case '/':
            return eval_ast(a->l) / eval_ast(a->r);

            // TODO: figure out how to assign registers to point to symbols in args
            /* variable and register asts */
        case 'v': case 'r':
            return (((struct symref *)a)->sym->value);
        case 'm':
            int memloc = eval_ast(((struct memref *)a)->loc);
            if (memloc < STACK_START || memloc > STACK_START + STACK_SIZE + 3) {
                fprintf(stderr, "error: attempted to access memory outside of stack range\n");
                return 0;
            }
            else {
                int tot = 0;
                for (int i = 0; i < 4; i++) {
                    tot += ((int) stack[memloc + i]) << ((3 - i) * 8);
                }
                return tot;
            }

        default:
            printf("bad nodetype %c\n", a->nodetype);
            return 0;
    }
}

void dump_ast(struct ast *a, int level) {

    printf("%*s", 2*level, "");	/* indent to this level */
    level++;

    if(!a) {
        printf("NULL\n");
        return;
    }

    switch(a->nodetype) {
        /* constant */
        case 'n':
            printf("number %4.4d\n", ((struct numval *)a)->number);
            return;

            /* expressions, comparisons, assignment*/
        case '+': case '-': case '*': case '/': case '=':
        case '1': case '2': case '3': case '4': case '5':
            printf("operation %c\n", a->nodetype);
            dump_ast(a->l, level);
            dump_ast(a->r, level);
            return;

            /* if statement, dump conditional and result */
        case 'i':
            printf("flow %c\n", a->nodetype);
            dump_ast( ((struct flow *)a)->cond, level);
            dump_ast( ((struct flow *)a)->then, level);
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
            dump_ast(((struct memref *)a)->loc, level);
            return;

        default:
            printf("bad nodetype %c\n", a->nodetype);
            return;
    }
}

int verify_ast(struct ast *a, struct sym_list *sl) {

    switch(a->nodetype) {
        /* constant */
        case 'n':
            return 1;

            /* expressions, comparisons, assignment*/
        case '+': case '-': case '*': case '/': case '=':
        case '1': case '2': case '3': case '4': case '5':
            return verify_ast(a->l, sl) && verify_ast(a->r, sl);

            /* if statement, check conditional and result */
        case 'i':
            return verify_ast(((struct flow *)a)->cond, sl) && verify_ast(((struct flow *)a)->then, sl);

            /* variable, register, and memory asts */
        case 'v':
        case 'r':
            return verify_ref(((struct symref *)a), sl);
        case 'm':
            return verify_ast(((struct memref *)a)->loc, sl);
        default:
            yyerror("bad nodetype");
            return 0;
    }
}

int verify_ref(struct symref *symr, struct sym_list *sl) {

    for (int i = 0; i < NUM_SPEC_REG; i++) {
        if (!strcmp(spec_reg[i].name, symr->name)) {
            symr->sym = &spec_reg[i];
            return 1;
        }
    }

    for (; sl; sl = sl->next)
    {
        if (!strcmp(sl->sym->name, symr->name)) {
            symr->sym = sl->sym;
            return 1;
        }
    }

    return 0;

}

// helper function, generates default built in registers,
// will eventually be replaced when custom registers are implemented
void generate_start_env() {

    /* 8 KB stack */
    STACK_START = 0;
    STACK_SIZE = 8192;

    /* 2 KB code (each instruction treated as 1 byte in simulation) */
    CODE_START = 0;
    CODE_SIZE = 2048;

    stack = malloc(sizeof (uint8_t) * STACK_SIZE - STACK_START);
    code = malloc(sizeof (char *) * CODE_SIZE - CODE_START);

    NUM_SPEC_REG = 4;
    NUM_GEN_REG = 8;

    spec_reg = malloc(sizeof (struct symbol) * NUM_SPEC_REG);
    gen_reg  = malloc(sizeof (struct symbol) * NUM_GEN_REG);

    spec_reg[0].name = strdup("AC");
    spec_reg[0].type = 0;
    spec_reg[0].value = 0;
    spec_reg[1].name = strdup("PC");
    spec_reg[1].type = 0;
    spec_reg[1].value = 0;
    spec_reg[2].name = strdup("SP");
    spec_reg[2].type = 0;
    spec_reg[2].value = 0;
    spec_reg[3].name = strdup("BP");
    spec_reg[3].type = 0;
    spec_reg[3].value = 0;

    for (int i = 0; i < NUM_GEN_REG; i++) {

        /* TODO: if NUM_GEN_REG > 10, need to change how this is handled */
        gen_reg[i].name = strdup("regx");
        gen_reg[i].name[3] = i + '0';

        gen_reg[i].type = 0;
        gen_reg[i].value = 0;

    }

}

int command_no;

void yyerror(char *s, ...) {
    va_list ap;
    va_start(ap, s);

    fprintf(stderr, "line %d: error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");

}

extern int yyparse(void);
extern void run_user();

int main(int argc, char **argv) {
    /* Process command line args*/
    yyin = fopen(argv[1], "r");

    /* generate default register and variable symbols */

    generate_start_env();

    yyparse();

    fclose(yyin);

    /*

    char *com_name = argv[2];

    struct command *c = get_command(com_name);

    struct ast_list *ca = c->actions;

    while (ca) {
        dumpast(ca->a, 0);
        ca = ca->next;
    }

    */

    run_user();

    return 0;
}
