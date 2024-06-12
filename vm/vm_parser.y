%{

#include "vm.h"
#include <stdint.h>


%}

%define parse.error detailed

%union {

    int d;
    char *strval;

}

%token <d> NUMBER
%token <strval> NAME
%token NEWLINE



%start system
%%

system: NAME NEWLINE NAME ':' NUMBER         { printf("test\n"); }




%%