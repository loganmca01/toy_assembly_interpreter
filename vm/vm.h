#include "../ast_header.h"

/* system globals */
extern int *registers;
extern uint8_t *memory
extern struct system_information system;

/* Flex and Bison functions/vars */
extern FILE *yyin;
extern int yylineno;
extern int yyparse(void);
void yyerror(char* s, ...);

/* VM functions */