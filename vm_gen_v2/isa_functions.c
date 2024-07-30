#include "isa.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

/* attributes */
int base-unit-width;
int minimum-decode;
int single-variant;
int decode-type;
char *decode-start;
int single-parcel;

void yyerror(char *s, ...) {
    va_list ap;
    va_start(ap, s);

    fprintf(stderr, "line %d: error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");

}


int main(int argc, char **argv) {

    /* Process command line args*/
    yyin = fopen(argv[1], "r");

    if (yyin == NULL) {
        fprintf(stderr, "error opening instruction set file\n");
        exit(1);
    }

    yyparse();

    fclose(yyin);

    return 0;
}
