#include <stdio.h>
#include "../ast_header.h"

void yyerror(char *s, ...);

extern struct system_information sys_info;

/* attributes */
extern int base-unit-width;
extern int minimum-decode;
extern int single-variant;
extern int decode-type;
extern char *decode-start;
extern int single-parcel;

struct variant_parcel {
    struct parcel_list *pl;
};

struct variant_noparcel {
    struct encoding_list *el;
};

struct parcel_list {
    struct parcel *p;
    struct parcel_list *next;
};

struct encoding_list {
    struct encoding *e;
    struct encoding_list *next;
};

struct parcel {
    int type;
    
};

struct encoding {
    char *name;
    int size;
};