#include "isa.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

/* attributes */
int register_size;
int number_of_registers;
int number_of_condition_codes;
int memory_size;
int base_unit;
int instruction_type;
int decode_type;
char *decode_start;
int instruction_length;

char **hw_names;

/* bitmask that tracks which attributes have been set */
int attribute_tracker;


int handle_attribute(int a_num, void *val) {
    switch (a_num) {
        case 0: {
            //printf("test0\n");
            int size = *((int *) val);
            if (attribute_tracker & REGISTER_SIZE_M) {
                yyerror("attribute already set");
                return 1;
            }
            if (size < 0 || size > 128) {
                yyerror("register size must be positive number between 8 and 128");
                return 1;
            }
            register_size = size;
            attribute_tracker |= REGISTER_SIZE_M;
            return 0;
        }
        case 1: {
            //printf("test1\n");
            int num = *((int *)val);
            if (attribute_tracker & NUMBER_OF_REGISTERS_M) {
                yyerror("attribute already set");
                return 1;
            }
            if (num <= 0 || num > 64) {
                yyerror("number of registers must be between 0 and 64");
                return 1;
            }
            if (attribute_tracker & NUMBER_OF_CONDITION_CODES_M) {
                hw_names = calloc((num + number_of_condition_codes), sizeof (char *));
            }
            number_of_registers = num;
            attribute_tracker |= NUMBER_OF_REGISTERS_M;
            return 0;
        }
        case 2: {
            //printf("test2\n");
            int num = *((int *)val);
            if (attribute_tracker & NUMBER_OF_CONDITION_CODES_M) {
                yyerror("attribute already set");
                return 1;
            }
            if (num < 0 || num > 16) {
                yyerror("number of condition codes must be between 0 and 16");
                return 1;
            }
            if (attribute_tracker & NUMBER_OF_REGISTERS_M) {
                hw_names = calloc((num + number_of_registers), sizeof (char *));
            }
            number_of_condition_codes = num;
            attribute_tracker |= NUMBER_OF_CONDITION_CODES_M;
            return 0;
        }
        case 3: {
            //printf("test3\n");
            int size = *((int *)val);
            if (attribute_tracker & MEMORY_SIZE_M) {
                yyerror("attribute already set");
                return 1;
            }
            if (size <= 0 || size > 32768) {
                yyerror("memory size must be between 0 and 32768 (32 KiB)");
                return 1;
            }
            memory_size = size;
            attribute_tracker |= MEMORY_SIZE_M;
            return 0;
        }
        case 4: {
            //printf("test4\n");
            int size = *((int *)val);
            if (attribute_tracker & BASE_UNIT_M) {
                yyerror("attribute already set");
                return 1;
            }
            if (size <= 0 || size > 16) {
                yyerror("base unit size must be between 0 and 16, the recommended size is 8 (1 byte)");
            }
            base_unit = size;
            attribute_tracker |= BASE_UNIT_M;
            return 0;
        }
        case 5: {
            //printf("test5\n");
            int type = *((int *)val);
            if (attribute_tracker & INSTRUCTION_TYPE_M) {
                yyerror("attribute already set");
                return 1;
            }
            if (type != 1 && type != 2) {
                yyerror("instruction type must be 1 (fixed width) or 2 (variable width)");
                return 1;
            }
            instruction_type = type;
            attribute_tracker |= INSTRUCTION_TYPE_M;
            return 0;
        }
        case 6: {
            //printf("test6\n");
            int type = *((int *)val);
            if (attribute_tracker & DECODE_TYPE_M) {
                yyerror("attribute already set");
                return 1;
            }
            if (!(attribute_tracker & INSTRUCTION_TYPE_M) || instruction_type == 2) {
                yyerror("decode type must be set after instruction type, and is only valid for instruction type 1");
                return 1;
            }
            if (type != 1 && type != 2) {
                yyerror("decode type must be 1 (decode from universal encoding section) or 2 (decode from bitmask)");
                return 1;
            }
            decode_type = type;
            attribute_tracker |= DECODE_TYPE_M;
            return 0;
        }
        case 7: {
            //printf("test7\n");
            char *str = *((char **)val);
            if (attribute_tracker & DECODE_START_M) {
                yyerror("attribute already set");
                return 1;
            }
            if (!(attribute_tracker & DECODE_TYPE_M) || decode_type == 2) {
                yyerror("decode start must be set after decode type, and is only valid for decode type 1");
            }
            /* no way to check that string is a valid name of an encoding in a format here, need to check later after formats set */
            decode_start = str;
            attribute_tracker |= DECODE_START_M;
            return 0;
        }
        case 8: {
            //printf("test8\n");
            int len = *((int *)val);
            if (attribute_tracker & INSTRUCTION_LENGTH_M) {
                yyerror("attribute already set");
                return 1;
            }
            if (!(attribute_tracker & INSTRUCTION_TYPE_M) || instruction_type == 2) {
                yyerror("instruction length must be set after instruction type, and is only valid for type 1 (fixed width) instruction sets");
                return 1;
            }
            instruction_length = len;
            attribute_tracker |= INSTRUCTION_LENGTH_M;
            return 0;
        }
    }
}

/* helper function for testing that attribute values were set correctly */
void print_attributes() {
    if (attribute_tracker & REGISTER_SIZE_M) {
        printf("register size: %d\n", register_size);
    }
    if (attribute_tracker & NUMBER_OF_REGISTERS_M) {
        printf("num regs: %d\n", number_of_registers);
    }
    if (attribute_tracker & NUMBER_OF_CONDITION_CODES_M) {
        printf("num cc: %d\n", number_of_condition_codes);
    }
    if (attribute_tracker & MEMORY_SIZE_M) {
        printf("mem size: %d\n", memory_size);
    }
    if (attribute_tracker & BASE_UNIT_M) {
        printf("base unit: %d\n", base_unit);
    }
    if (attribute_tracker & INSTRUCTION_TYPE_M) {
        printf("instr type: %d\n", instruction_type);
    }
    if (attribute_tracker & DECODE_TYPE_M) {
        printf("decode type: %d\n", decode_type);
    }
    if (attribute_tracker & DECODE_START_M) {
        printf("decode start: %s\n", decode_start);
    }
    if (attribute_tracker & INSTRUCTION_LENGTH_M) {
        printf("instr length: %d\n", instruction_length);
    }
    for (int i = 0; i < number_of_registers; i++) {
        if (hw_names[i] != NULL) printf("R[%d] = %s\n", i, hw_names[i]);
    }
    for (int i = number_of_registers; i < number_of_registers + number_of_condition_codes; i++) {
        if (hw_names[i] != NULL) printf("S[%d] = %s\n", i - number_of_registers, hw_names[i]);
    }
}

int verify_attribute_cover() {
    if (!(attribute_tracker & REGISTER_SIZE_M)) {
        yyerror("missing register_size attribute");
        return 1;
    } 
    else if (!(attribute_tracker & NUMBER_OF_REGISTERS_M)) {
        yyerror("missing number_of_registers attribute");
        return 1;
    }
    else if (!(attribute_tracker & NUMBER_OF_CONDITION_CODES_M)) {
        yyerror("missing number_of_condition_codes attribute");
        return 1;
    }
    else if (!(attribute_tracker & MEMORY_SIZE_M)) {
        yyerror("missing memory_size attribute");
        return 1;
    }
    else if (!(attribute_tracker & BASE_UNIT_M)) {
        yyerror("missing base_unit attribute");
        return 1;
    }
    else if (!(attribute_tracker & INSTRUCTION_TYPE_M)) {
        yyerror("missing instruction_type attribute");
        return 1;
    }
    else if (!(attribute_tracker & DECODE_TYPE_M) && (instruction_type == 1)) {
        yyerror("missing decode_type attribute (necessary for fixed width instruction sets)");
        return 1;
    }
    else if (!(attribute_tracker & DECODE_START_M) && (instruction_type == 1) && (decode_type == 1)) {
        yyerror("missing decode_start attribute (necessary for fixed width instruction sets of decode type 1)");
        return 1;
    }
    else if (!(attribute_tracker & DECODE_TYPE_M) && (instruction_type == 1)) {
        yyerror("missing instruction_length attribute (necessary for fixed width instruction sets)");
        return 1;
    }
    else return 0;
}

void **formats;

void yyerror(char *s, ...) {
    va_list ap;
    va_start(ap, s);

    fprintf(stderr, "line %d: error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
}


int main(int argc, char **argv) {


    int c, full;
    while ((c = getopt(argc, argv, "f:i:s:")) != -1)
	switch (c) {
        case 'f':
            yyin = fopen(optarg, "r");
            full = 1;
            break;
        case 'i':
        case 's':
            fprintf(stderr, "not yet supported\n");
            exit(1);
    }

    if (yyin == NULL) {
        fprintf(stderr, "error opening file\n");
        exit(1);
    }

    yyparse();

    print_attributes();

    fclose(yyin);

    return 0;
}

