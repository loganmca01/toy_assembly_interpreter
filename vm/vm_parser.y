%{

#include "vm.h"
#include <stdint.h>

int current_region = 0;

%}

%define parse.error detailed

%union {

    int d;
    char c;
    char *strval;
    struct ast_list *al;
    struct sym_list *sl;
    struct symbol *s;
    struct ast *a;

}

%token <d> NUMBER
%token <strval> NAME
%token NEWLINE

%token SYSTEM_L
%token REGISTER_COUNT_L
%token MEMORY_SIZE_L
%token NUMBER_OF_REGIONS_L
%token MEMORY_REGIONS_L
%token NUMBER_OF_INSTRUCTIONS_L
%token COMMAND_NO_L
%token NAME_L
%token OPCODE_L
%token ARGUMENTS_L

%type <al> action_list
%type <sl> arg_list
%type <s> arg
%type <a> tree node

%start system
%%

system: SYSTEM_L NEWLINE
        register_num
        register_list
        memory_info
        memory_regions
        instruction_num                         
        instruction_list                             {}
;

register_num:   REGISTER_COUNT_L NUMBER NEWLINE  
                    { 
                        sys_info.num_regs = $2;
                        sys_info.reg_names = arena_allocate(sizeof (char *) * $2);
                        registers = arena_allocate(sizeof (struct symbol) * $2);
                    }
;

register_list: register_name                    {}
            |  register_list register_name      {}
;
                                                    /* TODO: remove redundancy (named in 2 different places) */
register_name: NUMBER ',' NAME NEWLINE          { registers[$1].name = $3; sys_info.reg_names[$1] = $3; }
            |  NUMBER ',' NEWLINE               {}
;

memory_info: MEMORY_SIZE_L NUMBER NEWLINE
             NUMBER_OF_REGIONS_L NUMBER NEWLINE { sys_info.mem_size = $2; sys_info.num_regions = $5; sys_info.mem_regions = arena_allocate(sizeof (struct memory_region) * $5); }
;

memory_regions: MEMORY_REGIONS_L memory_region_list NEWLINE                 {}
;

memory_region_list: '[' NAME NUMBER NUMBER NUMBER ']'                       
                        { 
                            sys_info.mem_regions[current_region].name = $2;
                            sys_info.mem_regions[current_region].base = $3;
                            sys_info.mem_regions[current_region].bound = $4;
                            sys_info.mem_regions[current_region++].direction = $5;
                        }
                |   memory_region_list '[' NAME NUMBER NUMBER NUMBER ']'    
                        { 
                            sys_info.mem_regions[current_region].name = $3;
                            sys_info.mem_regions[current_region].base = $4;
                            sys_info.mem_regions[current_region].bound = $5;
                            sys_info.mem_regions[current_region++].direction = $6;
                        }
;

instruction_num: NUMBER_OF_INSTRUCTIONS_L NUMBER NEWLINE NEWLINE    { sys_info.num_instructions = $2; for (int i = 0; i < OPCODE_MAX; i++) instructions[i].opcode = 0; }
;

instruction_list: instruction                           {}
                | instruction_list instruction          {}
;

instruction:    COMMAND_NO_L NUMBER NEWLINE
                NAME_L NAME NEWLINE
                OPCODE_L NUMBER NEWLINE
                ARGUMENTS_L arg_list NEWLINE
                action_list NEWLINE                         
                    {   
                        //printf("%d\n", $8);
                        struct command i;
                        i.name = $5;
                        i.opcode = $8;
                        i.args = $11;
                        i.actions = $13;
                        instructions[$8] = i;
                    }
;

arg_list: /* */             { $$ = NULL; current_sl = $$; }
        | arg_list arg                                  
            {
                if ($1 == NULL) {
                    $$ = new_sym_list_a($2, NULL);
                    current_sl = $$;
                }
                else {
                    $$ = add_sym($1, new_sym_list_a($2, NULL));
                    current_sl = $$;
                }
            }
;

arg: '[' NUMBER NAME ']'                                { $$ = newsym_a($3, $2, 0); }
;

action_list: tree NEWLINE               { $$ = new_ast_list_a($1, NULL); }
        |    action_list tree NEWLINE   { $$ = add_ast($1, new_ast_list_a($2, NULL)); }
;

tree: node          { $$ = $1; }
    | tree node     { $$ = $1; }
;
    /* IDEA: change handling from individual characters to CHAR token (duh) */

node: '[' NAME ']'          
{
    //printf("%d test %s\n", yylineno, $2);
    struct ast *current = pop_stack();
    struct ast *next;
    switch ($2[0]) {
        case '=': case '+': case '-': case '*': case '/': case '|': case '&': case '^':
            next = newast_a($2[0], NULL, NULL);
            break;
        case 'i':
            next = newflow_a(NULL, NULL);
            break;
        case 'm':
            next = newmemref_a('m', NULL);
            break;
        default:
            yyerror("error rebuilding ast, make sure you're using an isa file generated through the isa_interpreter");
    }
    int check = handle_node(current, next, IS_PUSHED);
    if (check) yyerror("error rebuilding ast, make sure you're using an isa file generated through the isa_interpreter");
    $$ = next;
}
    | '[' NAME NAME ']'
{
    //printf("%d test2 %s %s\n", yylineno, $2, $3);
    struct ast *current = pop_stack();
    struct ast *next = newsymref_a($2[0], NULL, $3);
    int check = handle_node(current, next, IS_REF);
    if (check) yyerror("error rebuilding ast, make sure you're using an isa file generated through the isa_interpreter");
    $$ = next;
}
    | '[' NAME NUMBER ']'   
{
    //printf("%d test3 %s %d\n", yylineno, $2, $3);
    struct ast *current = pop_stack();
    struct ast *next = newnum_a($3);
    int check = handle_node(current, next, 0);
    if (check) yyerror("error rebuilding ast, make sure you're using an isa file generated through the isa_interpreter");
    $$ = next;
}
    | '[' NUMBER ']'   
{
    //printf("%d test4 %d\n", yylineno, $2);
    struct ast *current = pop_stack();
    struct ast *next = newcmp_a('0' + $2, NULL, NULL);
    int check = handle_node(current, next, IS_PUSHED);
    if (check) yyerror("error rebuilding ast, make sure you're using an isa file generated through the isa_interpreter");
    $$ = next;
}


%%