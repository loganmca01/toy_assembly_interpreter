#include "vm.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

int *registers;
uint8_t *memory;

struct system_information sys_info;

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

int main(int argc, char **argv) {

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

}
