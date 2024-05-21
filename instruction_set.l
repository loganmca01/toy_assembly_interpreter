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

(?i:define)                         { return DEFINE; }
(?i:stack)                          { return STACK; }
(?i:code)                           { return CODE; }
(?i:registers)                      { return REGS; }
(?i:pc_location)                    { return PC_LOC; }
(?i:literal_symbol)                 { return LITERAL_SYMBOL; }
(?i:register_symbol)                { return REGISTER_SYMBOL; }


[a-zA-Z][a-zA-Z0-9]+   {
                /* todo: add checking for register names in parser rather than lexer
                for (int i = 0; i < sys_info.num_regs; i++) {
                    if (!strcmp(sys_info.regs[i], yytext)) {
                        yyerror("%s is an existing register and can't be used as an argument", yytext);
                        return YYerror;
                    }
                }
                */

                yylval.strval = strdup(yytext);
                return NAME;
            }

-?[0-9]+    { yylval.d = atoi(yytext); return NUMBER; }
0[xX][0-9a-fA-F]+   { yylval.d = (int)strtol(yytext + 2, NULL, 16); return NUMBER; }
0[bB][01]+  { yylval.d = (int)strtol(yytext + 2, NULL, 2); return NUMBER; }

[ \t\r]     {}
\n          { return NEWLINE; }
.           { yylval.c = yytext[0]; return OTHER_CHAR; }

