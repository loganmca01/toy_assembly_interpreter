#include "vm.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

struct symbol *registers;
struct symbol PC;
uint8_t *memory;

struct system_information sys_info;
char *arena;
int arena_pointer;

struct command instructions[OPCODE_MAX];


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

    printf("\n");

    struct command c;

    for (int i = 0; i < OPCODE_MAX; i++) {

        c = instructions[i];

        if (c.name == NULL) continue;

        printf("name: %s\n", c.name);
        printf("opcode: 0x%02x\n", c.opcode);
        printf("arguments: ");

        for (struct sym_list *s = c.args; s != NULL; s = s->next) {
            printf("[%d %s]", s->sym->type, s->sym->name);
        }
        printf("\n");

        for (struct ast_list *a = c.actions; a != NULL; a = a->next) {
            dump_ast(stdout, a->a);
            printf("\n");
        }

        printf("\n");

    }

}

int main(int argc, char **argv) {

    PC.name = "PC";
    PC.value = 0;
    PC.type = 1;

    arena = calloc(sizeof (char), ARENA_SIZE);
    arena_pointer = 0;

    ast_stack = malloc(sizeof (struct ast *) * STACK_MAX);
    ast_stack_ptr = -1;

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



