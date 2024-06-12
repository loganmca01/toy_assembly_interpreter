#include "../ast_header.h"
#include <stdio.h>

/* system globals */
extern int *registers;
extern uint8_t *memory;
extern struct system_information sys_info;

/* Flex and Bison functions/vars */
extern FILE *yyin;
extern int yylineno;
extern int yyparse(void);
void yyerror(char* s, ...);

/* VM functions */