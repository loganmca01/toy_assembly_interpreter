
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "isa.h"

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
%token DEFINE_L ENCODING_L FUNCTION_L FIXED_L;
%token <d> REG CC

%nonassoc COND
%right ASSIGN LOG
%left <c> CMP
%right NEWLINE

%left '|' '&' '^'
%left '+' '-'
%left '*' '/'

%type <a> exp conditional action memory
%type <s> symbol
%type <sl> arg_list
%type <al> action_list

%start system
%%

system: named_reg_list opt_linebreak 
        named_condition_code_list opt_linebreak
        attribute_list opt_linebreak
        variant_list opt_linebreak
        instruction_list opt_linebreak              {}
;

linebreak: NEWLINE                                  {}
        |  NEWLINE linebreak                        {}
;

opt_linebreak: /* */                                {}
            |  linebreak                            {}
;

named_reg:  NAME '=' REG                            {}
;

named_reg_list: /* */                               {}
            |   named_reg_list named_reg linebreak  {}
;

named_condition_code:  NAME '=' CC                                         {}
;

named_condition_code_list: /* */                                            {}
            |   named_condition_code_list named_condition_code linebreak    {}
;

    /* todo : check that values make sense */
attribute: NAME ':' NUMBER              
{
    if (!strcmp($1, "base-unit-width")) {
        base-unit-width = $3;
    }
    else if (!strcmp($1, "minimum-decode")) {
        minimum-decode = $3;
    }
    else if (!strcmp($1, "single-variant")) {
        single-variant = $3;
    }
    else if (!strcmp($1, "decode-type")) {
        decode-type = $3;
    }
    else if (!strcmp($1, "single-parcel")) {
        single-parcel = $3;
    }
    else {
        yyerror("invalid attribute name %s", $1);
    }
}
       |   NAME ':' NAME                
{
    if (!strcmp($1, "decode-start")) {
        decode-start = $3;
    }
    else {
        /* TODO : fix so that it differentiates between invalid name and invalid format */
        yyerror("invalid attribute name")
    }
}
;

    /* todo, check that all required attributes are stated */
attribute_list: /* */                               {}
            | attribute_list attribute linebreak    {}
;

variant_list: /* "fixed" keyword for decode-type 2, requires bit values after length */    {}
            | variant_list variant linebreak        {}
;

variant: encoding_list                              {}
    |    parcel_list                                {}

encoding_list: encoding                 {}
            | encoding_list encoding    {}

parcel_list: parcel                     {}
        |    parcel_list parcel         {}

encoding: NAME '=' NUMBER                   {}
        | FIXED_L '=' NUMBER '=' NUMBER     {}

parcel:

instruction_list: /* nothing */                     { command_no = 1; }
    | instruction_list instruction linebreak        { command_no++; }
    | instruction_list instruction                  { command_no++; }
    | instruction_list error linebreak              {}
;

instruction: DEFINE_L NAME linebreak encoding_block linebreak function_block    {}
;

encoding_block: ENCODING_L opt_linebreak '{' decoding_list '}'            {}
;

decoding_list:

function_block: FUNCTION_L opt_linebreak '{' action_list '}'              {}

