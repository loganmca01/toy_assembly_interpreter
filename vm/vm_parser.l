%option noyywrap nodefault yylineno

%{

#include "vm.h"
#include "vm_parser.tab.h"

%}

%%

"+" |
"-" |
"*" |
"/" |
"|" |
"&" |
"^" |
"=" |
"(" |
")" |
"{" |
"}" |
"[" |
"]" |
"," |
":" |
";"         { return yytext[0]; }

[a-zA-Z%][a-zA-Z0-9\-]*   {

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