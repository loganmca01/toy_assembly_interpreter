%option noyywrap nodefault yylineno

%{

#include "vm.h"
#include "vm_parser.tab.h"

%}

%%


"(" |
")" |
"{" |
"}" |
"[" |
"]" |
"," |
":" |
";"             { return yytext[0]; }

SYSTEM                  { return SYSTEM_L; }
register-count:         { return REGISTER_COUNT_L; }
memory-size:            { return MEMORY_SIZE_L; }
number-of-regions:      { return NUMBER_OF_REGIONS_L; }
memory-regions:         { return MEMORY_REGIONS_L; }
number-of-instructions: { return NUMBER_OF_INSTRUCTIONS_L; }
COMMAND-NO              { return COMMAND_NO_L; }
name:                   { return NAME_L; }
opcode:                 { return OPCODE_L; }
arguments:              { return ARGUMENTS_L; }

    /* todo: replace strdup with function using arena_allocate */
[a-zA-Z%+\-\*\/&\|=\^][a-zA-Z0-9\-]*   {

                yylval.strval = strdup(yytext);
                return NAME;
            }

-?[0-9]+    { yylval.d = atoi(yytext); return NUMBER; }
0[xX][0-9a-fA-F]+   { yylval.d = (int)strtol(yytext + 2, NULL, 16); return NUMBER; }
0[bB][01]+  { yylval.d = (int)strtol(yytext + 2, NULL, 2); return NUMBER; }

[ \t\r]     {}
\n          { return NEWLINE; }
.           { yyerror("unknown character %c", yytext[0]); }

%%