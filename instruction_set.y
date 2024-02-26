

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
    struct command *cmd;
    char *strval;
    char c;
    double d;
}

%token <d> NUMBER
%token <strval> NAME
%token <s> REG
%token NEWLINE
%token DEFINE
%token <s> VAR
%token PC AC SP BP

%nonassoc COND
%right ASSIGN
%nonassoc <c> CMP

%left '+' '-'
%left '*' '/'

%type <cmd> command
%type <s> register
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
;

    // can be any special purpose or general purpose register
register: REG           { $$ = $1; }
    | AC                { $$ = &symtab[52]; }
    | PC                { $$ = &symtab[53]; }
    | SP                { $$ = &symtab[54]; }
    | BP                { $$ = &symtab[55]; }
;

    // memory reference
memory: '(' exp ')'        { $$ = newmemref('m', $2); }

;

   // list of register and variable arguments
arg_list:    /* nothing */      { $$ = NULL; }
    | arg_list register         { if ($1) { $1->next = new_sym_list($2, NULL); $$ = $1;} else { $$ = new_sym_list($2, NULL); } }
    | arg_list VAR              { if ($1) { $1->next = new_sym_list($2, NULL); $$ = $1;} else $$ = new_sym_list($2, NULL); }
;

    // expression - any number, memory reference, register, variable, operation, or comparison
exp: exp CMP exp                { $$ = newcmp($2, $1, $3); }
    | exp '+' exp               { $$ = newast('+', $1, $3); }
    | exp '-' exp               { $$ = newast('-', $1, $3); }
    | exp '*' exp               { $$ = newast('*', $1, $3); }
    | exp '/' exp               { $$ = newast('*', $1, $3); }
    | VAR                       { $$ = newsymref('v', $1); }
    | register                  { $$ = newsymref('r', $1); }
    | memory                    { $$ = $1; }
    | NUMBER                    { $$ = newnum($1); }
;

    // conditional expression, helps with action
conditional: COND exp           { $$ = newflow($2, NULL); }
;

    // some variety of assignment operation, ends with semicolon
action: register ASSIGN exp ';'            { $$ = newast('=', newsymref('r', $1), $3); }
    | memory ASSIGN exp ';'                { $$ = newast('=', $1, $3); }
    | register ASSIGN exp conditional ';'  { struct ast *front = newast('=', newsymref('r', $1), $3);
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



