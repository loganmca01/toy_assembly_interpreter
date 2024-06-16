%{

#include "vm.h"
#include <stdint.h>

int current_region = 0;
int current_instruction = 0;



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
%token <c> CHAR
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
        instruction_list                        {}
;

register_num:   REGISTER_COUNT_L NUMBER NEWLINE  
                    { 
                        sys_info.num_regs = $2;
                        sys_info.reg_names = arena_allocate(sizeof (char *) * $2);
                        for (int i = 0; i < $2; i++) sys_info.reg_names[i] = NULL;
                    }
;

register_list: register_name                    {}
            |  register_list register_name      {}
;

register_name: NUMBER ',' NAME NEWLINE          { sys_info.reg_names[$1] = $3; }
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

                        current_instruction++;
                    }
;

arg_list: /* */             { $$ = NULL; }
        | arg_list arg                                  
            {
                if ($1 == NULL) {
                    $$ = new_sym_list_a($2, NULL);
                }
                else {
                    $$ = add_sym($1, new_sym_list_a($2, NULL));
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

node: '[' CHAR ']'          { $$ = NULL; }
    | '[' CHAR NAME ']'     { $$ = NULL; }
    | '[' CHAR NUMBER ']'   { $$ = NULL; }
;

%%