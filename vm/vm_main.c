#include "vm.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>

struct symbol *registers;
struct symbol PC;
uint8_t *memory;

struct system_information sys_info;
char *arena;
int arena_pointer;

struct command instructions[OPCODE_MAX];


/* prints ast to file, used for testing */
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

/* organization function to help declutter main */
void setup_defaults() {
    
    PC.name = "PC";
    PC.value = 0;
    PC.type = 1;

    arena = calloc(sizeof (char), ARENA_SIZE);
    arena_pointer = 0;

    ast_stack = malloc(sizeof (struct ast *) * STACK_MAX);
    ast_stack_ptr = -1;
}

int main(int argc, char **argv) {

    setup_defaults();

    char buff[64];

    strcpy(buff, "../");

    if (strlen(argv[1]) > 60) {
        fprintf(stderr, "error: filename too long\n");
        return 1;
    }

    strcat(buff, argv[1]);

    yyin = fopen(argv[1], "r");

    if (yyin == NULL) {
        fprintf(stderr, "error opening isa file\n");
        exit(1);
    }

    yyparse();

    fclose(yyin);

    /* useful for testing 

    print_system_info();

    printf("%d\n", arena_pointer);

    printf("waiting on connection\n");

    */

    int status;
    struct in_msgbuf in;

    /* wait on open connection, returns -1 if error other than unknown msg queue id */
    while ((status = open_connection()) == 1);
    if (status == -1) return 1;
    
    printf("interface connected\n");

    send_message("successfully connected to VM\n");

    for (;;) {

        if (receive_message(&in)) return 1;

        if (handle_command(in.mtext) == 0) {
            break;
        }
        
    }


}

int handle_command(char *command) {

    fprintf(stderr, "%s\n", command);
    if (!strcmp(command, "quit")) return 0;
    
    char *mask = command;

    while (*mask && *mask != ' ') mask++;
    if (*mask != '\0') *mask = '\0';

    if (!strcmp(command, "load")) {
        if (run_load(mask + 1)) {
            fprintf(stderr, "error opening load file.\n");
            return -1;
        }
    }

    return 1;
}

int run_load(char *filename) {

    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }
}