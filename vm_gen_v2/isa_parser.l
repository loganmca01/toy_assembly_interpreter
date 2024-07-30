%option noyywrap nodefault yylineno

%{
#include "isa.h"
#include "isa.tab.h"
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
"&&"        { yylval.c = '7'; return LOG; }
"||"        { yylval.c = '8'; return LOG; }

":="       { return ASSIGN; }

":"        { return COND; }

(?i:define)                         { return DEFINE_L; }
(?i:encoding)                       { return ECONDING_L; }
(?i:function)                       { return FUNCTION_L; }
(?i:fixed)                          { return FIXED_L; }

S\[[0-9]+\]                         {
                                        /* TODO */
                                        return CC;
                                    }

R\[[0-9]+\]                         { 
                                        int sum = 0;
                                        for (int i = 2; yytext[i] != ']'; i++) {                                        
                                            sum *= 10 * (i - 2);
                                            sum += yytext[i] - '0';
                                        }
                                        if (sum < 0 || sum >= sys_info.num_regs)
                                            yyerror("named register out of bounds");
                                        else {
                                            yylval.d = sum;
                                            return REG;
                                        }
                                    }

[a-zA-Z%][a-zA-Z0-9\.]*   {

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