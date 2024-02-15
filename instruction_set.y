

%{
#include <stdio.h>
#include <stdlib.h>
#include "instruction_set.h"

%}



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

command_list: /* nothing */         {  }
    | command_list command NEWLINE  {  }
    | command_list command          {  }
;

command: DEFINE NAME arg_list '{' action_list '}'
                        { add_command($2, $5, $3); }
;

register: REG           { $$ = $1; }
    | AC                { $$ = &symtab[52]; }
    | PC                { $$ = &symtab[53]; }
    | SP                { $$ = &symtab[54]; }
    | BP                { $$ = &symtab[55]; }
;

memory: '(' register ')'        { $$ = newmemref('M', newsymref('r', $2), newnum(0)); }
    | '(' register '+' VAR ')'  { $$ = newmemref('M', newsymref('r', $2), newsymref('v', $4)); }
    | '(' register '-' VAR ')'  { $$ = newmemref('m', newsymref('r', $2), newsymref('v', $4)); }
    | '(' register '+' NUMBER ')'  { $$ = newmemref('M', newsymref('r', $2), newnum($4)); }
    | '(' register '-' NUMBER ')'  { $$ = newmemref('m', newsymref('r', $2), newnum($4)); }
    | '(' VAR ')'               { $$ = newmemref('M', newsymref('v', $2), newnum(0)); }
    | '(' VAR '+' VAR ')'  { $$ = newmemref('M', newsymref('v', $2), newsymref('v', $4)); }
    | '(' VAR '-' VAR ')'  { $$ = newmemref('m', newsymref('v', $2), newsymref('v', $4)); }
    | '(' VAR '+' NUMBER ')'  { $$ = newmemref('M', newsymref('v', $2), newnum($4)); }
    | '(' VAR '-' NUMBER ')'  { $$ = newmemref('m', newsymref('v', $2), newnum($4)); }
;

arg_list:    /* nothing */      { $$ = NULL; }
    | arg_list register         { if ($1) { $1->next = new_sym_list($2, NULL); $$ = $1;} else { $$ = new_sym_list($2, NULL); } }
    | arg_list VAR              { if ($1) { $1->next = new_sym_list($2, NULL); $$ = $1;} else $$ = new_sym_list($2, NULL); }
;

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

conditional: COND exp           { $$ = newflow($2, NULL); }
;

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

action_list: action                 { $$ = new_ast_list($1, NULL); }
    | action_list NEWLINE action    { $$ = add_ast($1, new_ast_list($3, NULL)); }
;


%%



