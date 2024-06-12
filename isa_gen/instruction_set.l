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
";"         { return yytext[0]; }

">"         { yylval.c = '1'; return CMP; }
"<"         { yylval.c = '2'; return CMP; }
"=="        { yylval.c = '3'; return CMP; }
">="        { yylval.c = '4'; return CMP; }
"<="        { yylval.c = '5'; return CMP; }
"!="        { yylval.c = '6'; return CMP; }
"&&"        { yylval.c = '7'; return CMP; }
"||"        { yylval.c = '8'; return CMP; }

":="       { return ASSIGN; }

":"        { return COND; }

(?i:define)                         { return DEFINE; }

[a-zA-Z]+:[a-zA-Z/]+                { 
                                        char *mask = yytext;
                                        while (*mask && *mask != ':') mask++;

                                        /* separate type from name */
                                        *mask = '\0';
                                        mask++;

                                        int type;

                                        /* check type of argument */
                                        if (!strcmp(mask, SYM0)) type = 0;
                                        else if (!strcmp(mask, SYM1)) type = 1;
                                        else if (!strcmp(mask, SYM2)) type = 2;
                                        else if (!strcmp(mask, SYM3)) type = 3;
                                        else if (!strcmp(mask, SYM4)) type = 4;
                                        else if (!strcmp(mask, SYM5)) type = 5;
                                        else if (!strcmp(mask, SYM6)) type = 6;
                                        else {
                                            printf("\ntest %s\n", mask);
                                            yyerror("invalid type, arguments should be in form \"name:r/m/i\" \nwhere r/m/i represent which types (register, memory, or immediate) the argument can be ");
                                            type = -1;
                                        }

                                        yylval.s = newsym(strdup(yytext), type); 
                                        return ARG_SYMBOL; 
                                    }

R\[[0-9]+\]                         { 
                                        /* TODO: check that register number is in range, add a type to REG and set it */
                                        int sum = 0;
                                        for (int i = 2; yytext[i] != ']'; i++) {                                        
                                            sum *= 10 * (i - 2);
                                            sum += yytext[i] - '0';
                                        }
                                        if (sum >= sys_info.num_regs)
                                            yyerror("named register out of bounds");
                                        else {
                                            yylval.d = sum;
                                            return REG;
                                        }
                                    }

[a-zA-Z%][a-zA-Z0-9]*   {
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
.           { yyerror("unknown character %c", yytext[0]); }

