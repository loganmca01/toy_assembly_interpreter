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
                /* TODO: add check for gen reg name, throw error */
                for (int i = 0; i < NUM_SPEC_REG; i++) {
                    if (!strcmp(spec_reg[i].name, yytext)) {
                        yylval.s = &spec_reg[i];
                        return REG;
                    }
                }
                for (int i = 0; i < NUM_GEN_REG; i++) {
                    if (!strcmp(gen_reg[i].name, yytext)) {
                        yyerror("unavailable dummy register name %s", yytext);
                        return YYerror;
                    }
                }
                if (!strncmp(yytext, "reg", 3)) {
                    yylval.s = newsym(strdup(yytext), 0);
                    return REG;
                }
                if (!strncmp(yytext, "var", 3)) {
                    yylval.s = newsym(strdup(yytext), 1);
                    return VAR;
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

