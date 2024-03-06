
#include "instruction_set.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

/* TODO: replace these all with globals (possibly global system_info struct) and allow instruction set to set values for each */

/* 8 KB stack */
#define STACK_SIZE 8192
#define STACK_START 0

/* 2 KB code (each instruction treated as 1 byte in simulation) */
#define CODE_SIZE 2048
#define CODE_START 0

#define INPUT_SIZE 256
#define NUM_SPEC_REG 4
#define NUM_GEN_REG 8

char stack[STACK_SIZE];
char *code[CODE_SIZE];

int reg[NUM_GEN_REG + NUM_SPEC_REG];

/* tracks next available address in code array - not always same as PC in case of jumps */
int code_index;

void run_user() {

    print_welcome();

    printf("> ");

    char input_store[INPUT_SIZE];
    size_t size = sizeof(char) * INPUT_SIZE;

    char *input = &input_store[0];

    // for edge case testing (user enters line more than 256 bytes) add second pointer to spot, compare two.

    int done = 0;

    while(getline(&input, &size, stdin)) {

        while (*input == ' ') input++;

        input[strcspn(input, "\r\n")] = '\0';

        /* move past leading spaces */
        while(*input == ' ') input++;

        if (*input == '.') done = run_dot(input);

        if (done) break;

        printf("> ");

    }

}

void print_welcome() {

    /* TODO: replace macros with globals once registers and code regions can be varied */
    printf("Welcome to the toy assembly interpreter.\n");
    print_system_info();

    printf("For information about the dot commands available to you use .help\n\n");

    printf("Keep in mind that dot commands are not loaded into code memory, and should\n"
           "be ignored when calculating instruction locations for jumps.\n\n");

    printf("Use .quit to quit the program\n\n\n");

}

void print_spec_regs() {

    for (int i = 0; i < NUM_SPEC_REG; i++) {
        printf("%s", symtab[i + 52].name);
        if (i + 1 != NUM_SPEC_REG) printf(", ");
        else printf("\n");
    }

}

void print_system_info() {

    printf("System information:\n\n");
    printf("%d Special purpose registers: ", NUM_SPEC_REG);
    print_spec_regs();
    printf("%d General purpose registers: reg0 - reg%d\n\n", NUM_GEN_REG, NUM_GEN_REG - 1);

    printf("Stack: %d bytes, starting at address %d\nCode:  %d bytes, starting at address %d\n\n", STACK_SIZE, STACK_START, CODE_SIZE, CODE_START);

}

void print_help_message() {

    print_system_info();

    /* TODO: for print_stack, add another option to print bytes vs words? */

    printf("Dot commands available:\n\n");
    printf(".help                                         you're here now\n\n");
    printf(".print_stack [start address] [end address]    print the contents of stack between two addresses\n\n");
    printf(".print_code [start address] [end address]     print the contents of code memory between two addresses\n\n");
    printf(".clear_stack [start address] [end address]    clear the contents of the stack between two addresses\n\n");
    printf(".clear_code [start address] [end address]     clear the contents of code memory between two addresses\n\n");
    printf(".clear_all                                    clear everything from stack and code memory\n\n");
    printf(".file [filepath]                              run commands from file, only need file name if in same folder\n\n");

    printf("For language specific commands look in text document passed to interpreter\n\n\n");

}

int run_dot(char *input) {

    char *mask = input;
    strsep(&mask, " ");

    if (!strcmp(input, ".print_stack")) {
        while (mask && *mask == ' ') mask++;
        run_print(1, mask);
    }
    else if (!strcmp(input, ".print_code")) {
        while (mask && *mask == ' ') mask++;
    }
    else if (!strcmp(input, ".clear_stack")) {
        while (mask && *mask == ' ') mask++;
    }
    else if (!strcmp(input, ".clear_code")) {
        while (mask && *mask == ' ') mask++;
    }
    else if (!strcmp(input, ".clear_all")) {
        while (mask && *mask == ' ') mask++;
        run_clear(2, mask);
    }
    else if (!strcmp(input, ".help")) {
        print_help_message();
    }
    else if (!strcmp(input, ".quit")) {
        return 1;
    }
    else {
        /* TODO: add "help" command, possibly with arguments for section and tell user to call it */
        fprintf(stderr, "error: invalid dot command\n");
    }
    return 0;
}

/*
 * Print contents of stack or code memory
 * Called by run_dot
 *
 * mode 0 = code
 *      1 = stack
 *
 * function is long but most of it is just checking error cases
 *
 */
void run_print(int type, char *args) {

    /* type is not 0, so printing stack */
    /* TODO: possibly print out differently depending on word size, maybe require 32 bit word size? tbd */
    /* TODO: consider, should I give errors from issues with alignment? or is that an unnecessary complication */
    if (type) {
        char *mask = args;

        strsep(&mask, " ");

        if (!mask) {
            fprintf(stderr, "error: invalid number of parameters, proper form .print_stack [start address] [end address]\n");
            return;
        }

        char *endptr;

        errno = 0;
        long start_val = strtol(args, &endptr, 0);

        /* two error cases, not a number and number out of range of stack */
        if (errno || *endptr) {
            fprintf(stderr, "error: parameters for .print_stack must be numbers in base 10 or base 16 prepended with 0x\n");
            return;
        }
        else if (start_val < STACK_START || start_val > STACK_START + STACK_SIZE) {
            fprintf(stderr, "error: parameters for .print_stack must be within stack range %d to %d\n", STACK_START, STACK_SIZE + STACK_START);
            return;
        }

        char *mask2 = mask;
        strsep(&mask2, " ");

        /* check for a third argument, trailing spaces allowed */
        if (mask2) {
            while(*mask2 == ' ') mask2++;
            if(*mask2) {
                fprintf(stderr, "error: invalid number of parameters, proper form .print_stack [start address] [end address]\n");
                return;
            }
        }

        errno = 0;
        long end_val = strtol(mask, &endptr, 0);

        /* same error cases as first arg, added case for arg less than first */
        if (errno || *endptr) {
            fprintf(stderr, "error: parameters for .print_stack must be numbers in base 10 or base 16 prepended with 0x\n");
            return;
        }
        else if (end_val < STACK_START || end_val > STACK_START + STACK_SIZE) {
            fprintf(stderr, "error: parameters for .print_stack must be within stack range %d to %d\n", STACK_START, STACK_SIZE + STACK_START);
            return;
        }
        else if (end_val < start_val) {
            fprintf(stderr, "error: second parameter of .print_stack must be >= first\n");
            return;
        }

        /* print starting address if it's not divisible by 4 (if it is, it will be printed in loop)
         *
         * shouldn't be possible to overflow conversion or mess up sign as stack
         * start and range will always be positive and within bounds of int
         *
         */
        if (start_val % 4) printf("0x%.4x: ", (unsigned int) start_val);


        for (long l = start_val; l <= end_val; l++) {

            if ((l % 4) == 0) {
                printf("\n0x%.4x: ", (unsigned int) l);
            }

            print_bin(stack[l]);

            printf(" ");

        }

        printf("\n");

    }
    else {

    }
}

void print_bin(char byte_val) {
    char buff[9];
    buff[8] = '\0';

    for (int i = 7; i >= 0; i--) {

        if (byte_val % 2) buff[i] = '1';
        else buff[i] = '0';
    }

    printf("%s", &buff[0]);

}

/*
 * Zero out stack and/or code memory
 *
 * mode 0 = clear code
 *      1 = clear stack
 *      2 = clear all of both
 *
 */
void run_clear(int type, char *args) {

}


