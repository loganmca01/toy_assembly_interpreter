#include <stdio.h>
#include "../ast_header.h"

/* attributes */
extern int register_size;
extern int number_of_registers;
extern int number_of_condition_codes;
extern int memory_size;
extern int base_unit;
extern int instruction_type;
extern int decode_type;
extern char *decode_start;
extern int instruction_length;

extern char **hw_names;

/* bitmask that tracks which attributes have been set */
extern int attribute_tracker;

/* helper macros for checking attributes */
#define REGISTER_SIZE_M             (0b1)
#define NUMBER_OF_REGISTERS_M       (0b10)
#define NUMBER_OF_CONDITION_CODES_M (0b100)
#define MEMORY_SIZE_M               (0b1000)
#define BASE_UNIT_M                 (0b10000)
#define INSTRUCTION_TYPE_M          (0b100000)
#define DECODE_TYPE_M               (0b1000000)
#define DECODE_START_M              (0b10000000)
#define INSTRUCTION_LENGTH_M        (0b100000000)


int handle_attribute(int a_num, void *val);
void print_attributes();
int verify_attribute_cover();

/* format buffer, casted to appropriate pointer type based on instruction_type */
extern void **formats;

struct format_var {
    struct parcel_list *pl;
};

struct format_fixed {
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
    /*
    * 1 - fixed size, non-optional parcel
    * 2 - fixed size, optional parcel
    * 3 - variable sized block, no encodings
    */
    int type;
    int *sizes;
};

struct encoding {
    char *name;
    int size;
};

void yyerror(char* s, ...);
extern int yyparse(void);

extern int yylineno;
extern FILE *yyin;