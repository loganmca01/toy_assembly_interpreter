

%{
#include <stdio.h>
#include <stdlib.h>
#include "instruction_set.h"

%}

%define parse.error detailed

%union {
    struct ast *a;
    struct ast_list *al;
    struct symbol *s;
    struct sym_list *sl;
    //struct command *cmd;
    char *strval;
    char c;
    double d;
}

%token <d> NUMBER
%token <strval> NAME
%token <strval> REG
%token NEWLINE
%token DEFINE
%token <strval> VAR

%nonassoc COND
%right ASSIGN
%nonassoc <c> CMP

%left '+' '-'
%left '*' '/'

%type <cmd> command
%type <a> exp conditional action memory
%type <sl> arg_list
%type <al> action_list

%start command_list
%%




    // command list - starting symbol, entire input file should be part of it
command_list: /* nothing */         { command_no = 1; }
    | command_list command NEWLINE  { command_no++; }
    | command_list command          { command_no++; }
    | command_list error NEWLINE    {  }

;
    /* todo: second rule for no arg_list, then remove empty from arglist to allow for comma separation */
    // command - made up of definition, argument and list of actions in curly braces
command: DEFINE NAME arg_list '{' action_list '}'
                        {
                            int check = 0, count = 0;

                            for (struct ast_list *mask = $5; mask; mask = mask->next)
                            {
                                count++;
                                if (!verify_ast(mask->a, $3))
                                {
                                    yyerror("use of undefined dummy var/reg in command %d action %d", command_no, count);
                                    check = 1;
                                    break;
                                }
                            }

                            if (!check)
                            {
                                add_command($2, $5, $3);
                            }
                        }
      | DEFINE NAME '{' action_list '}'
                        {
                            int check = 0, count = 0;

                            for (struct ast_list *mask = $4; mask; mask = mask->next)
                            {
                                count++;
                                if (!verify_ast(mask->a, NULL))
                                {
                                    yyerror("use of undefined var/reg in command %d action %d", command_no, count);
                                    check = 1;
                                    break;
                                }
                            }

                            if (!check)
                            {
                                add_command($2, $4, NULL);
                            }
                        }
;


    // memory reference
memory: '[' exp ']'        { $$ = newmemref('m', $2); }
;

   // list of register and variable arguments
arg_list: REG                   { $$ = new_sym_list(newsym($1, 0), NULL); }
    | VAR                       { $$ = new_sym_list(newsym($1, 1), NULL); }
    | arg_list ',' REG          { $$ = add_sym($1, new_sym_list(newsym($3, 0), NULL)); }
    | arg_list ',' VAR          { $$ = add_sym($1, new_sym_list(newsym($3, 1), NULL)); }
;

    // expression - any number, memory reference, register, variable, operation, or comparison
exp: exp CMP exp                { $$ = newcmp($2, $1, $3); }
    | exp '+' exp               { $$ = newast('+', $1, $3); }
    | exp '-' exp               { $$ = newast('-', $1, $3); }
    | exp '*' exp               { $$ = newast('*', $1, $3); }
    | exp '/' exp               { $$ = newast('*', $1, $3); }
    | '(' exp ')'               { $$ = $2; }
    | VAR                       { $$ = newsymref('v', NULL, $1); }
    | REG                       { $$ = newsymref('r', NULL, $1); }
    | memory                    { $$ = $1; }
    | NUMBER                    { $$ = newnum($1); }
;

    // conditional expression, helps with action
conditional: COND exp           { $$ = newflow($2, NULL); }
;
    // TODO: figure out how to assign registers to point to symbols in args
    // some variety of assignment operation, ends with semicolon
action: REG ASSIGN exp ';'            { $$ = newast('=', newsymref('r', NULL, $1), $3); }
    | memory ASSIGN exp ';'           { $$ = newast('=', $1, $3); }
    | REG ASSIGN exp conditional ';'  { struct ast *front = newast('=', newsymref('r', NULL, $1), $3);
                                             ((struct flow *) $4)->then = front;
                                             $$ = $4;
                                      }
    | memory ASSIGN exp conditional ';'    { struct ast *front = newast('=', $1, $3);
                                             ((struct flow *) $4)->then = front;
                                             $$ = $4;
                                           }
;

    // list of actions, each action must be on a different line
action_list: action                 {
                                        $$ = new_ast_list($1, NULL);
                                    }
    | NEWLINE action                { $$ = new_ast_list($2, NULL); }
    | action_list NEWLINE action    { $$ = add_ast($1, new_ast_list($3, NULL)); }
    | action_list NEWLINE           { $$ = $1; }
;


%%



