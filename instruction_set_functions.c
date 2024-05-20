//
// Created by logan on 2/3/2024.
//

#include "instruction_set.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

struct command command_table[MAX_COMMAND];
int num_commands;

struct system_information sys_info;

void add_command(char *name, struct ast_list *actions, struct sym_list *args) {

    if (num_commands == MAX_COMMAND) {
        yyerror("command table overflow. max 1024 allowed\n");
        return;
    }

    int i;
    struct command c;

    for (i = 0; i < num_commands; i++) {

        c = command_table[i];

        /* command found */
        if(!strcmp(c.name, name)) {
            yyerror("command with name %s already defined\n", name);
            return;
        }
        else if (strcmp(c.name, name) > 0) {
            for (int j = num_commands; j > i; j--) {
                command_table[j] = command_table[j - 1];
            }
            command_table[i].name = name;
            command_table[i].args = args;
            command_table[i].actions = actions;
            return;
        }
    }

    command_table[i].name = name;
    command_table[i].args = args;
    command_table[i].actions = actions;

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

    for (int i = 0; i < sys_info.num_spec_reg; i++) {
        free(sys_info.spec_regs[i]);
    }
    for (int i = 0; i < sys_info.num_gen_reg; i++) {
        free(sys_info.gen_regs[i]);
    }
}

void command_table_free() {

    for (int i = 0; i < num_commands; i++) {

        free(command_table[i].name);
        ast_list_free(command_table[i].actions);
        sym_list_free(command_table[i].args);

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

    /* todo: replace NUM_SPEC_REG with appropriate part of system_info global struct */
    for (int i = 0; i < sys_info.num_spec_reg; i++) {
        if (!strcmp(sys_info.spec_regs[i], symr->name)) {
            return 1;
        }
    }

    for (; sl; sl = sl->next)
    {
        if (!strcmp(sl->sym->name, symr->name)) {
            return 1;
        }
    }

    return 0;

}

// helper function, generates default built in registers,
// will eventually be replaced when custom registers are implemented
void generate_default_system() {

    /* 8 KB stack */
    sys_info.stack_start = 0;
    sys_info.stack_size = 8192;

    /* 2 KB code (each instruction treated as 1 byte in simulation) */
    sys_info.code_start = 0;
    sys_info.code_size = 2048;

    sys_info.num_spec_reg = 4;
    sys_info.num_gen_reg = 8;

    sys_info.spec_regs = malloc(sizeof (char *) * sys_info.num_spec_reg);
    sys_info.gen_regs = malloc(sizeof (char *) * sys_info.num_gen_reg);

    sys_info.spec_regs[0] = strdup("AC");

    sys_info.spec_regs[1] = strdup("PC");

    sys_info.spec_regs[2] = strdup("SP");

    sys_info.spec_regs[3] = strdup("BP");


    for (int i = 0; i < sys_info.num_gen_reg; i++) {

        /* TODO: if NUM_GEN_REG > 10, need to change how this is handled */
        sys_info.gen_regs[i] = strdup("regx");
        sys_info.gen_regs[i][3] = i + '0';

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

int main(int argc, char **argv) {

    if (argc != 3 || strcmp(&argv[2][strlen(argv[2]) - 4], ".isa")) {
        fprintf(stderr, "error: invalid number of arguments. correct usage: ./isa_interpreter [instruction_set] [output file name (extension .isa)]\n");
    }

    /* Process command line args*/
    yyin = fopen(argv[1], "r");

    /* generate default system information, can be altered in passed rtn file */
    generate_default_system();

    yyparse();

    fclose(yyin);

    FILE *out = fopen(argv[2], "w");

    if (errno > 0) {
        fprintf(stderr,"error opening output file\n");
        exit(1);
    }



    //fprintf(out, "testing\n");

    fclose(out);

    return 0;
}
