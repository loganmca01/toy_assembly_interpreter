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

}
