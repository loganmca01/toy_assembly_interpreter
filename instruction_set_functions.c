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
            num_commands++;
            return;
        }
    }

    command_table[i].name = name;
    command_table[i].args = args;
    command_table[i].actions = actions;
    num_commands++;

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

struct ast *newnum(int d)
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

void reg_table_free() {

    for (int i = 0; i < sys_info.num_regs; i++) {
        free(sys_info.regs[i]);
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

/* prints ast to file following a BFS */
void dump_ast(FILE *f, struct ast *a) {

    switch(a->nodetype) {
        /* constant */
        case 'n':
            fprintf(f, "[n %d]", ((struct numval *)a)->number);
            return;

            /* expressions, comparisons, assignment*/
        case '+': case '-': case '*': case '/': case '=':
        case '1': case '2': case '3': case '4': case '5':
            fprintf(f, "[%c]", a->nodetype);
            dump_ast(f, a->l);
            dump_ast(f, a->r);
            return;

            /* if statement, dump conditional and result */
        case 'i':
            fprintf(f, "[i]");
            dump_ast(f, ((struct flow *)a)->cond);
            dump_ast(f, ((struct flow *)a)->then);
            return;

            /* variable, register, and memory asts */
        case 'v':
            fprintf(f, "[v %s]", ((struct symref *)a)->name);
            return;
        case 'r':
            fprintf(f, "[r %s]", ((struct symref *)a)->name);
            return;
        case 'm':
            fprintf(f, "[m]");
            dump_ast(f, ((struct memref *)a)->loc);
            return;

        default:
            fprintf(stderr, "bad nodetype %c\n", a->nodetype);
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

    for (int i = 0; i < sys_info.num_regs; i++) {
        if (!strcmp(sys_info.regs[i], symr->name)) {
            symr->nodetype = 'r';
            return 1;
        }
    }

    for (; sl; sl = sl->next)
    {
        if (!strcmp(sl->sym->name, symr->name)) {
            if (symr->nodetype == 'r') {
                sl->sym->type = 1;
            }
            return 1;
        }
    }

    return 0;

}

// helper function, generates default built in registers,
// will eventually be replaced when custom registers are implemented
void generate_default_system() {

    sys_info.mem_size = 8192;

    /* '0' represents no character, will be checked in user env program */
    sys_info.reg_sym = '0';
    sys_info.lit_sym = '0';

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

    if (argc != 4 || strcmp(&argv[3][strlen(argv[2]) - 4], ".isa")) {
        fprintf(stderr, "error: invalid number of arguments. correct usage: ./isa_interpreter [instruction_set] [output file name (extension .isa)]\n");
    }

    FILE *sys_file = fopen(argv[1], "r");

    if (sys_file == NULL) {
        fprintf(stderr, "error opening system information file\n");
        exit(1);
    }






    num_commands = 0;

    /* Process command line args*/
    yyin = fopen(argv[2], "r");

    if (yyin == NULL) {
        fprintf(stderr, "error opening instruction set file\n");
        exit(1);
    }

    /* generate default system information, can be altered in passed rtn file */
    generate_default_system();

    yyparse();

    fclose(yyin);

    FILE *out = fopen(argv[2], "w");

    if (errno > 0) {
        fprintf(stderr,"error opening output file\n");
        exit(1);
    }

    struct command c;

    fprintf(out, "SYSTEM\n");

    fprintf(out, "register-count: %d\n", sys_info.num_regs);
    fprintf(out, "register-names: ");

    for (int i = 0; i < sys_info.num_regs; i++) {
        fprintf(out, "%s ", sys_info.regs[i]);
    }

    fprintf(out, "\nprogram-counter-location: %d\n", sys_info.pc_loc);

    fprintf(out, "memory-size: %d\n", sys_info.mem_size);

    fprintf(out, "number-of-instructions: %d\n", num_commands);


    /*
    fprintf(out, "literal-value-symbol: %c\n", sys_info.lit_sym);
    fprintf(out, "register-value-symbol: %c\n", sys_info.reg_sym);
    */

    for (int i = 0; i < num_commands; i++) {

        fprintf(out, "\nCOMMAND-NO %d\n", i+1);

        c = command_table[i];

        fprintf(out, "command-name: %s\n", c.name);
        fprintf(out, "command-arguments: ");

        for (struct sym_list *s = c.args; s != NULL; s = s->next) {
            fprintf(out, "[%d %s]", s->sym->type, s->sym->name);
        }
        fprintf(out, "\n");

        for (struct ast_list *a = c.actions; a != NULL; a = a->next) {
            dump_ast(out, a->a);
            fprintf(out, "\n");
        }


    }

    fclose(out);

    return 0;
}
