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



%start system
%%

system: NAME ':' NUMBER         { printf("test\n"); }




%%