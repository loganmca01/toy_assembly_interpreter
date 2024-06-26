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

uint8_t low_opcode = 1;

/* 
*   helper function, returns 0 if opcode is available, 1 if not
*/
int check_opcode(int opcode) {

    int check = 0;

    for (int i = 0; i < num_commands; i++) {
        if (command_table[i].opcode == opcode) {
            check = 1;
            break;
        }
    }

    return check;

}

void add_command(char *name, struct ast_list *actions, struct sym_list *args, uint8_t opcode) {

    if (num_commands == MAX_COMMAND) {
        yyerror("command table overflow. max 1024 allowed\n");
        return;
    }

    for (int i = 0; i < num_commands; i++) {
        /* command found */

        if(!strcmp(command_table[i].name, name)) {
            yyerror("command with name %s already defined\n", name);
            return;
        }
    }

    uint8_t checked_opcode;

    if (opcode == 0) {
        checked_opcode = low_opcode;
        low_opcode++;
        while (check_opcode(low_opcode)) 
            low_opcode++;
    }
    else {
        if (check_opcode(opcode)) {
            yyerror("opcode already taken (may be taken by auto generated opcode, if so move command higher up)");
            return;
        }
        checked_opcode = opcode;
        
        if (opcode == low_opcode) {
            low_opcode++;
            while (check_opcode(low_opcode)) 
                low_opcode++;
        }

    }

    struct command c;
    int i;

    for (i = 0; i < num_commands; i++) {

        c = command_table[i];

        /* order commands by opcode */
        if (c.opcode > checked_opcode) {
            for (int j = num_commands; j > i; j--) {
                command_table[j] = command_table[j - 1];
            }
            command_table[i].name = name;
            command_table[i].args = args;
            command_table[i].actions = actions;
            command_table[i].opcode = checked_opcode;
            num_commands++;
            return;
        }
    }

    command_table[i].name = name;
    command_table[i].args = args;
    command_table[i].actions = actions;
    command_table[i].opcode = checked_opcode;
    num_commands++;

}


void command_table_free() {

    for (int i = 0; i < num_commands; i++) {

        free(command_table[i].name);
        ast_list_free(command_table[i].actions);
        sym_list_free(command_table[i].args);

    }
}

/* prints ast to file following a BFS */
void dump_ast(FILE *f, struct ast *a) {

    switch(a->nodetype) {
        /* constant */
        case 'n':
            fprintf(f, "[n %d]", ((struct numval *)a)->number);
            return;

            /* expressions, comparisons, assignment*/
        case '+': case '-': case '*': case '/': case '=': case '|': case '&': case '^':
        case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8':
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
        case 'a':
            fprintf(f, "[a %s]", ((struct symref *)a)->name);
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
            return 0;

        /* assignment */
        case '=':
            if (a->l->nodetype == 'v') a->l->nodetype == 'a';
            return verify_ast(a->l, sl) | verify_ast(a->r, sl);

            /* expressions, comparisons*/
        case '+': case '-': case '*': case '/': case '|': case '&': case '^':
        case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8':
            return verify_ast(a->l, sl) | verify_ast(a->r, sl);

            /* if statement, check conditional and result */
        case 'i':
            return verify_ast(((struct flow *)a)->cond, sl) | verify_ast(((struct flow *)a)->then, sl);

            /* variable, register, and memory asts */
        case 'v':
        case 'a':
            return verify_ref(((struct symref *)a), sl);
        case 'm':
            return verify_ast(((struct memref *)a)->loc, sl);
        default:
            yyerror("bad nodetype");
            return 1;
    }
}

int verify_ref(struct symref *symr, struct sym_list *sl) {

    if (!strcmp("PC", symr->name)) {
        return 0;
    }

    for (int i = 0; i < sys_info.num_regs; i++) {
        if (sys_info.reg_names[i] && !strcmp(sys_info.reg_names[i], symr->name)) {
            return 0;
        }
    }

    for (; sl; sl = sl->next)
    {
        if (!strcmp(sl->sym->name, symr->name)) {

            /* if the symref is to an argument, and it's being assigned to, make sure that it can't be a literal */
            if (symr->nodetype == 'a') {
                if (sl->sym->type == 2 | sl->sym->type == 4 | sl->sym->type == 5 | sl->sym->type == 6) {
                    yyerror("assignment to argument %s that can take immediate value", symr->name);
                    return 2;
                }
            }

            return 0;

        }
    }
    yyerror("reference to unknown register/variable %s", symr->name);
    return 1;

}

// helper function, currently just sets up a buffer for memory regions
void generate_default_system() {

    /* buffer for memory regions to be loaded initially */
    sys_info.mem_regions = malloc(sizeof (struct memory_region) * 8);

}

int command_no;

void yyerror(char *s, ...) {
    va_list ap;
    va_start(ap, s);

    fprintf(stderr, "line %d: error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");

}


int main(int argc, char **argv) {

    if (argc != 4 || strcmp(&argv[3][strlen(argv[3]) - 4], ".isa")) {
        fprintf(stderr, "error: invalid number of arguments. correct usage: ./isa_interpreter [system info file] [instruction_set] [output file name (extension .isa)]\n");
    }

    FILE *sys_file = fopen(argv[1], "r");

    if (sys_file == NULL) {
        fprintf(stderr, "error opening system information file\n");
        exit(1);
    }

    generate_default_system();

    parse_system_info(sys_file);

    num_commands = 0;

    /* Process command line args*/
    yyin = fopen(argv[2], "r");

    if (yyin == NULL) {
        fprintf(stderr, "error opening instruction set file\n");
        exit(1);
    }

    yyparse();

    fclose(yyin);

    FILE *out = fopen(argv[3], "w");

    if (errno > 0) {
        fprintf(stderr,"error opening output file\n");
        exit(1);
    }

    struct command c;

    fprintf(out, "SYSTEM\n");

    fprintf(out, "register-count: %d\n", sys_info.num_regs);

    for (int i = 0; i < sys_info.num_regs; i++) {
        if (sys_info.reg_names[i])
            fprintf(out, "%d, %s\n", i, sys_info.reg_names[i]);
        else
            fprintf(out, "%d\n", i);
    }

    fprintf(out, "memory-size: %d\n", sys_info.mem_size);

    fprintf(out, "number-of-regions: %d\n", sys_info.num_regions);

    fprintf(out, "memory-regions:");

    for (int i = 0; i < sys_info.num_regions; i++) {
        fprintf(out, " [%s %d %d %d]", sys_info.mem_regions[i].name, sys_info.mem_regions[i].base, sys_info.mem_regions[i].bound, sys_info.mem_regions[i].direction);
    }

    fprintf(out, "\n");

    fprintf(out, "number-of-instructions: %d\n", num_commands);

    for (int i = 0; i < num_commands; i++) {

        fprintf(out, "\nCOMMAND-NO %d\n", i+1);

        c = command_table[i];

        fprintf(out, "name: %s\n", c.name);
        fprintf(out, "opcode: 0x%02x\n", c.opcode);
        fprintf(out, "arguments: ");

        for (struct sym_list *s = c.args; s != NULL; s = s->next) {
            fprintf(out, "[%d %s]", s->sym->type, s->sym->name);
        }
        fprintf(out, "\n");

        for (struct ast_list *a = c.actions; a != NULL; a = a->next) {
            dump_ast(out, a->a);
            fprintf(out, "\n");
        }
    }
    fprintf(out, "\n");

    fclose(out);

    return 0;
}
