

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "isa_gen/instruction_set.h"

char *tmp[64];

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
    int d;
}

%token <d> NUMBER
%token <strval> NAME
%token NEWLINE
%token DEFINE
%token <d> REG

%nonassoc COND
%right ASSIGN
%nonassoc <c> CMP
%right NEWLINE

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
                                if (!verify_ast(mask->a, $3))
                                {
                                    yyerror("use of undefined register/variable in command %d action %d", command_no, count);
                                    check = 1;
                                    break;
                                }
                            }

                            if (!check)
                            {
                                add_command($2, $6, $3);
                            }
                        }
      | DEFINE NAME opt_linebreak '{' action_list '}'
                        {
                            int check = 0, count = 0;

                            for (struct ast_list *mask = $5; mask; mask = mask->next)
                            {
                                count++;
                                if (!verify_ast(mask->a, NULL))
                                {
                                    yyerror("use of undefined register/variable in command %d action %d", command_no, count);
                                    check = 1;
                                    break;
                                }
                            }

                            if (!check)
                            {
                                add_command($2, $5, NULL);
                            }
                        }
;


    // memory reference
memory: '[' exp ']'        { $$ = newmemref('m', $2); }
;

symbol: NAME               { $$ = newsym($1, 0); }
;

   // list of register and variable arguments
arg_list: symbol                { $$ = new_sym_list($1, NULL); }
    | arg_list ',' symbol       { $$ = add_sym($1, new_sym_list($3, NULL)); }
;

    // expression - any number, memory reference, register, variable, operation, or comparison
exp: exp CMP exp                { $$ = newcmp($2, $1, $3); }
    | exp '+' exp               { $$ = newast('+', $1, $3); }
    | exp '-' exp               { $$ = newast('-', $1, $3); }
    | exp '*' exp               { $$ = newast('*', $1, $3); }
    | exp '/' exp               { $$ = newast('*', $1, $3); }
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
action: symbol ASSIGN exp ';'           { $$ = newast('=', newsymref('r', NULL, $1->name), $3); free($1); }
    | memory ASSIGN exp ';'             { $$ = newast('=', $1, $3); }
    | symbol ASSIGN exp conditional ';' { struct ast *front = newast('=', newsymref('r', NULL, $1->name), $3);
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



