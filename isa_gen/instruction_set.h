#include <stdio.h>
#include "../ast_header.h"

extern struct system_information sys_info;

void generate_default_system();

#define MAX_COMMAND 256

// add - adds command to symbol table
// get - returns pointer to command from table
void add_command(char *name, struct ast_list *actions, struct sym_list *args, uint8_t opcode);

// might not be necessary, these would probably only be called
// when user is exiting simulation
void command_table_free();

void dump_ast(FILE *f, struct ast *a);
int verify_ast(struct ast *a, struct sym_list *sl);
int verify_ref(struct symref *symr, struct sym_list *sl);

// parser for system info file
int parse_system_info(FILE *f);

// from flex, bison
extern FILE *yyin;
extern int yylineno;
extern int command_no;
void yyerror(char* s, ...);
