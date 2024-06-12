#include "vm.h"
#include <stdint.h>

int *registers;
uint8_t *memory

struct system_information system;

int main(int argc, char **argv) {

    yyin = fopen(argv[2], "r");

    if (yyin == NULL) {
        fprintf(stderr, "error opening instruction set file\n");
        exit(1);
    }

    yyparse();

    fclose(yyin);

}