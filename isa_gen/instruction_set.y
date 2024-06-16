

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "instruction_set.h"

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
%token DEFINE
%token <d> REG

%nonassoc COND
%right ASSIGN
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

system: named_reg_list opt_linebreak command_list   {  }

named_reg:  NAME '=' REG                            { sys_info.reg_names[$3] = strdup($1); }

named_reg_list: /* */                               {  }
            |   named_reg_list named_reg linebreak  {  }

linebreak: NEWLINE                                  {}
        |  NEWLINE linebreak                        {}
;

opt_linebreak: /* */                                {}
            |  linebreak                            {}
;

    // command list - starting symbol, entire input file should be part of it
command_list: /* nothing */             { command_no = 1; }
    | command_list command linebreak    { command_no++; }
    | command_list command              { command_no++; }
    | command_list error linebreak      {}
;

    // command - made up of definition, argument and list of actions in curly braces
command: DEFINE NAME arg_list opt_linebreak '{' action_list '}'
                        {
                            int check = 0, count = 0;

                            for (struct ast_list *mask = $6; mask; mask = mask->next)
                            {
                                count++;
                                int errcode;
                                if (errcode = verify_ast(mask->a, $3))
                                {
                                    check = 1;
                                    break;
                                }
                            }

                            if (!check)
                            {
                                add_command($2, $6, $3, 0);
                            }
                        }
      | DEFINE NAME opt_linebreak '{' action_list '}'
                        {
                            int check = 0, count = 0;

                            for (struct ast_list *mask = $5; mask; mask = mask->next)
                            {
                                count++;
                                if (verify_ast(mask->a, NULL))
                                {
                                    
                                    check = 1;
                                    break;
                                }
                            }

                            if (!check)
                            {
                                add_command($2, $5, NULL, 0);
                            }
                        }
      | DEFINE NUMBER NAME arg_list opt_linebreak '{' action_list '}'
                        {
                            int check = 0, count = 0;

                            for (struct ast_list *mask = $7; mask; mask = mask->next)
                            {
                                count++;
                                if (verify_ast(mask->a, $4))
                                {
                                    
                                    check = 1;
                                    break;
                                }
                            }

                            if (!check)
                            {
                                add_command($3, $7, $4, $2);
                            }
                        }
      | DEFINE NUMBER NAME opt_linebreak '{' action_list '}'
                        {
                            int check = 0, count = 0;

                            for (struct ast_list *mask = $6; mask; mask = mask->next)
                            {
                                count++;
                                if (verify_ast(mask->a, NULL))
                                {
                                    
                                    check = 1;
                                    break;
                                }
                            }

                            if (!check)
                            {
                                add_command($3, $6, NULL, $2);
                            }
                        }
;


    // memory reference
memory: '[' exp ']'        { $$ = newmemref('m', $2); }
;

symbol: NAME               { $$ = newsym($1, 0, 0); }
;

   // list of register and variable arguments
arg_list: ARG_SYMBOL                { $$ = new_sym_list($1, NULL); }
    | arg_list ',' ARG_SYMBOL       { $$ = add_sym($1, new_sym_list($3, NULL)); }
;

    // expression - any number, memory reference, register, variable, operation, or comparison
exp:  exp CMP exp               { $$ = newcmp($2, $1, $3); }
    | exp '+' exp               { $$ = newast('+', $1, $3); }
    | exp '-' exp               { $$ = newast('-', $1, $3); }
    | exp '*' exp               { $$ = newast('*', $1, $3); }
    | exp '/' exp               { $$ = newast('*', $1, $3); }
    | exp '|' exp               { $$ = newast('|', $1, $3); }
    | exp '&' exp               { $$ = newast('&', $1, $3); }
    | exp '^' exp               { $$ = newast('^', $1, $3); }
    | '(' exp ')'               { $$ = $2; }
    | symbol                    { $$ = newsymref('v', NULL, $1->name); free($1); }
    | memory                    { $$ = $1; }
    | NUMBER                    { $$ = newnum($1); }
;

    // conditional expression, helps with action
conditional: COND exp           { $$ = newflow($2, NULL); }
;

    // TODO: figure out how to assign registers to point to symbols in args
    // some variety of assignment operation, ends with semicolon
action: symbol ASSIGN exp ';'           { $$ = newast('=', newsymref('a', NULL, $1->name), $3); free($1); }
    | memory ASSIGN exp ';'             { $$ = newast('=', $1, $3); }
    | symbol ASSIGN exp conditional ';' { struct ast *front = newast('=', newsymref('a', NULL, $1->name), $3);
                                          ((struct flow *) $4)->then = front;
                                          $$ = $4;
                                          free($1);
                                        }
    | memory ASSIGN exp conditional ';' { struct ast *front = newast('=', $1, $3);
                                          ((struct flow *) $4)->then = front;
                                          $$ = $4;
                                        }
;

    // list of actions, each action must be on a different line
action_list: action                     {
                                            $$ = new_ast_list($1, NULL);
                                        }
    | linebreak action                  { $$ = new_ast_list($2, NULL); }
    | action_list linebreak action      { $$ = add_ast($1, new_ast_list($3, NULL)); }
    | action_list linebreak             { $$ = $1; }
;


%%



