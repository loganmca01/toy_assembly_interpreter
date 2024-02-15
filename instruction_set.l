%option noyywrap nodefault yylineno

%{
#include "instruction_set.h"
#include "instruction_set.tab.h"
%}

%%

"+" |
"-" |
"*" |
"/" |
"(" |
")" |
"{" |
"}" |
";"         { return yytext[0]; }

">"         { yylval.c = '1'; return CMP; }
"<"         { yylval.c = '2'; return CMP; }
"=="        { yylval.c = '3'; return CMP; }
">="        { yylval.c = '4'; return CMP; }
"<="        { yylval.c = '5'; return CMP; }

":="       { return ASSIGN; }

":"        { return COND; }

reg[a-z]            { yylval.s = &symtab[yytext[3] - 97]; return REG; }

(?i:define)         { return DEFINE; }
var[a-z]            { yylval.s = &symtab[yytext[3] - 71]; return VAR; }

PC          { return PC; }
AC          { return AC; }
SP          { return SP; }
BP          { return BP; }

[a-zA-Z]+   { yylval.strval = strdup(yytext); return NAME; }

-?[0-9]+    { yylval.d = atoi(yytext); return NUMBER; }
0[xX][0-9a-fA-F]+   { yylval.d = (int)strtol(yytext + 2, NULL, 16); return NUMBER; }
0[bB][01]+  { yylval.d = (int)strtol(yytext + 2, NULL, 2); return NUMBER; }

[ \t\r]     {}
\n          { return NEWLINE; }
.           { printf("unknown character %c\n", *yytext); }

