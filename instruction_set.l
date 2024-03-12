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

(?i:define)         { return DEFINE; }

[a-zA-Z]+   {
                for (int i = 0; i < NUM_SYM; i++) {
                    if (!strcmp(symtab[i].name, yytext)) {
                        yylval.s = &symtab[i];
                        return REG;
                    }
                }
                if (!strncmp(yytext, "reg", 3)) {
                    yylval.s = newsym(strdup(yytext), 0);
                    return REG;
                }
                if (!strncmp(yytext, "var", 3)) {
                    yylval.s = newsym(strdup(yytext), 1);
                    return REG;
                }
                yylval.strval = strdup(yytext);
                return NAME;
            }

-?[0-9]+    { yylval.d = atoi(yytext); return NUMBER; }
0[xX][0-9a-fA-F]+   { yylval.d = (int)strtol(yytext + 2, NULL, 16); return NUMBER; }
0[bB][01]+  { yylval.d = (int)strtol(yytext + 2, NULL, 2); return NUMBER; }

[ \t\r]     {}
\n          { return NEWLINE; }
.           { printf("unknown character %c\n", *yytext); }

