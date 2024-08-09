%option noyywrap nodefault yylineno

%{
#include "isa.h"
#include "isa_parser.tab.h"
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

">"         { yylval.c = '1'; return CMP; }
"<"         { yylval.c = '2'; return CMP; }
"=="        { yylval.c = '3'; return CMP; }
">="        { yylval.c = '4'; return CMP; }
"<="        { yylval.c = '5'; return CMP; }
"!="        { yylval.c = '6'; return CMP; }
"&&"        { yylval.c = '7'; return LOG; }
"||"        { yylval.c = '8'; return LOG; }

":="       { return ASSIGN; }


    /* labels for attributes and other keywords */
(?i:define)                         { return DEFINE_L; }
(?i:formats)                        { return FORMATS_L; }
(?i:encoding)                       { return ENCODING_L; }
(?i:function)                       { return FUNCTION_L; }
(?i:fixed)                          { return FIXED_L; }
(?i:size)                           { return SIZE_L; }
"SYSTEM-ATTRIBUTES"                 { return SYSTEM_L; }
"LANGUAGE-ATTRIBUTES"               { return LANGUAGE_L; }
"register_size"                     { yylval.d = 0; return ATTRIBUTE_L; }
"number_of_registers"               { yylval.d = 1; return ATTRIBUTE_L; }
"number_of_condition_codes"         { yylval.d = 2; return ATTRIBUTE_L; }
"memory_size"                       { yylval.d = 3; return ATTRIBUTE_L; }
"base_unit"                         { yylval.d = 4; return ATTRIBUTE_L; }
"instruction_type"                  { yylval.d = 5; return ATTRIBUTE_L; }
"decode_type"                       { yylval.d = 6; return ATTRIBUTE_L; }
"decode_start"                      { yylval.d = 7; return ATTRIBUTE_L; }
"instruction_length"                { yylval.d = 8; return ATTRIBUTE_L; }


(?i:named_hardware)                 { return NAMED_HARDWARE_L; }


S\[[0-9]+\]                         {
                                        yylval.strval = strndup((yytext + 2), (strlen(yytext) - 2));
                                        yylval.strval[strlen(yylval.strval) - 1] = '\0';
                                        return CC;
                                    }

R\[[0-9]+\]                         { 
                                        //fprintf(stderr, "%s %s\n", yytext, yytext+2);
                                        yylval.strval = strndup((yytext + 2), (strlen(yytext) - 2));
                                        yylval.strval[strlen(yylval.strval) - 1] = '\0';
                                        return REG;
                                    }

[a-zA-Z%][a-zA-Z0-9\.]*   {

                yylval.strval = strdup(yytext);
                //printf("%s\n", yytext);
                return NAME;
            }

-?[0-9]+    { yylval.d = atoi(yytext); return NUMBER; }
0[xX][0-9a-fA-F]+   { yylval.d = (int)strtol(yytext + 2, NULL, 16); return NUMBER; }
0[bB][01]+  { yylval.d = (int)strtol(yytext + 2, NULL, 2); return NUMBER; }

[ \t\r]     {}
\n          { return NEWLINE; }
.           { yyerror("unknown character %c", yytext[0]); }

%%