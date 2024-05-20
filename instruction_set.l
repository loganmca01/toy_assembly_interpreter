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
"[" |
"]" |
"," |
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
                for (int i = 0; i < sys_info.num_spec_reg; i++) {
                    if (!strcmp(sys_info.spec_regs[i], yytext)) {
                        yylval.strval = strdup(yytext);
                        return REG;
                    }
                }
                for (int i = 0; i < sys_info.num_gen_reg; i++) {
                    if (!strcmp(sys_info.gen_regs[i], yytext)) {
                        yyerror("unavailable dummy register name %s", yytext);
                        return YYerror;
                    }
                }
                if (!strncmp(yytext, "reg", 3)) {
                    yylval.strval = strdup(yytext);
                    return REG;
                }
                if (!strncmp(yytext, "var", 3)) {
                    yylval.strval = strdup(yytext);
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

