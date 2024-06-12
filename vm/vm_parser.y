%{

#include "vm.h"
#include <stdint.h>


%}

%define parse.error detailed

%union {

    int d;
    char *strval;

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

%start system
%%

system: SYSTEM_L NEWLINE
        register_num
        register_list
        memory_info
        memory_regions
        instruction_num                         
        instruction_list                        {  }
;

register_num:  REGISTER_COUNT_L NUMBER NEWLINE  { 
                                                    sys_info.num_regs = $2;
                                                    sys_info.reg_names = malloc(sizeof (char *) * $2);
                                                    for (int i = 0; i < $2; i++) sys_info.reg_names[i] = NULL;
                                                }
;

register_list: register_name                    {}
            |  register_list register_name      {}
;

register_name: NUMBER ',' NAME NEWLINE          { sys_info.reg_names[$1] = $3; }
;

memory_info: MEMORY_SIZE_L NUMBER NEWLINE
             NUMBER_OF_REGIONS_L NUMBER NEWLINE { sys_info.mem_size = $2; sys_info.num_regions = $5; }





%%