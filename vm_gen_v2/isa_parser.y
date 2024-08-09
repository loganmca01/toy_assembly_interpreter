
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "isa.h"
#include <errno.h>

%}

%define parse.error detailed

%union {
    struct ast *a;
    struct ast_list *al;
    struct symbol *s;
    struct sym_list *sl;
    char *strval;
    char c;
    int d;
}

%token <d> NUMBER
%token <strval> NAME
%token <s> ARG_SYMBOL
%token NEWLINE
%token DEFINE_L ENCODING_L FUNCTION_L FIXED_L SYSTEM_L LANGUAGE_L NAMED_HARDWARE_L;
%token <d> ATTRIBUTE_L
%token <strval> REG CC

%nonassoc COND
%right ASSIGN LOG
%left <c> CMP
%right NEWLINE

%left '|' '&' '^'
%left '+' '-'
%left '*' '/'


%start system
%%

system: attribute_list                              {  }
;

linebreak: NEWLINE                                  {}
        |  NEWLINE linebreak                        {}
;

opt_linebreak: /* */                                {}
            |  linebreak                            {}
;

attribute_list: attribute opt_linebreak             {  }
    |       attribute_list attribute opt_linebreak  {  }
;

attribute: ATTRIBUTE_L ':' NUMBER linebreak                         { if ($1 == 7) {yyerror("attribute value must be a string"); YYERROR;} if (handle_attribute($1, &$3)) YYERROR; }
        |  ATTRIBUTE_L ':' NAME linebreak                           { if ($1 == 7) {yyerror("attribute value must be a number"); YYERROR;} if (handle_attribute($1, &$3)) YYERROR; }
        |  NAMED_HARDWARE_L ':' opt_linebreak named_hardware_list   {  }
;

named_hardware_list: named_hardware                         { if (!((attribute_tracker & NUMBER_OF_REGISTERS_M) && (attribute_tracker && NUMBER_OF_CONDITION_CODES_M))) yyerror("must set number of registers and condition codes before naming them"); }
                |    named_hardware_list named_hardware     {}
;

named_hardware: REG '=' NAME linebreak         
                {
                    int num;
                    if (((!(num = atoi($1))) && strcmp($1, "0")) || num < 0 || num >= number_of_registers) {
                        //fprintf(stderr, "test");
                        yyerror("invalid register number in named hardware list %s", $1);
                        YYERROR;
                    }
                    else hw_names[num] = $3;
                }
            |   CC '=' NAME linebreak       
                {
                    int num;
                    if (((!(num = atoi($1))) && strcmp($1, "0")) || num < 0 || num >= number_of_condition_codes) {
                        //printf("%d\n", num);
                        yyerror("invalid cc number in named hardware list");
                        YYERROR;
                    }
                    else hw_names[num + number_of_registers] = $3;
                }