
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

unsigned char stack[STACK_SIZE];
char *code[CODE_SIZE];

/* TODO: when adding custom registers to instruction set, allow for different sizes (and possibly different word size)
 * TODO ALSO: maybe assign all registers numbers and add conversion, then check for correct reg size array by number
 *
 * TODO IMPORTANT: get special purpose register table from other file, this one should only be gen purpose
 */
struct symbol *reg[NUM_GEN_REG + NUM_SPEC_REG];

/* tracks next available address in code array - not always same as PC in case of jumps */
int code_index;

void run_user() {

    print_welcome();

    /*
     * temporary test for dot commands till instructions can run
     */

    stack[0] = 25;
    stack[3] = 166;
    stack[8] = 3;

    code[0] = "test1";
    code[5] = "test2";
    code[26] = "test3";
    code[21] = "test4";

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
        else {
            int check = run_instruction(input);
        }

        if (done) break;

        /* TODO: check that PC is within bounds, while it doesn't point to NULL, loop and keep running instructions */

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
        run_print(0, mask);
    }
    else if (!strcmp(input, ".clear_stack")) {
        while (mask && *mask == ' ') mask++;
        run_clear(1, mask);
    }
    else if (!strcmp(input, ".clear_code")) {
        while (mask && *mask == ' ') mask++;
        run_clear(0, mask);
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

    char *mask = args;

    char *messages[2];
    messages[0] = "print_code";
    messages[1] = "print_stack";

    strsep(&mask, " ");

    if (!mask) {
        fprintf(stderr, "error: invalid number of parameters, proper form .%s [start address] [end address]\n", messages[type]);
        return;
    }

    char *endptr;

    errno = 0;
    long start_val = strtol(args, &endptr, 0);

    /* two error cases, not a number and number out of range of stack */
    if (errno || *endptr) {
        fprintf(stderr, "error: parameters for .%s must be numbers in base 10 or base 16 prepended with 0x\n", messages[type]);
        return;
    }
    else if (type && (start_val < STACK_START || start_val > STACK_START + STACK_SIZE)) {
        fprintf(stderr, "error: parameters for .print_stack must be within stack range %d to %d\n", STACK_START, STACK_SIZE + STACK_START);
        return;
    }
    else if (!type && (start_val < CODE_START || start_val > CODE_START + CODE_SIZE)) {
        fprintf(stderr, "error: parameters for .print_code must be within code range %d to %d\n", CODE_START, CODE_START + CODE_SIZE);
        return;
    }

    char *mask2 = mask;
    strsep(&mask2, " ");

    /* check for a third argument, trailing spaces allowed */
    if (mask2) {
        while(*mask2 == ' ') mask2++;
        if(*mask2) {
            fprintf(stderr, "error: invalid number of parameters, proper form .%s [start address] [end address]\n", messages[type]);
            return;
        }
    }

    errno = 0;
    long end_val = strtol(mask, &endptr, 0);

    /* same error cases as first arg, added case for arg less than first */
    if (errno || *endptr) {
        fprintf(stderr, "error: parameters for .%s must be numbers in base 10 or base 16 prepended with 0x\n", messages[type]);
        return;
    }
    else if (type && (end_val < STACK_START || end_val > STACK_START + STACK_SIZE)) {
        fprintf(stderr, "error: parameters for .print_stack must be within stack range %d to %d\n", STACK_START, STACK_SIZE + STACK_START);
        return;
    }
    else if (!type && (end_val < CODE_START || end_val > CODE_START + CODE_SIZE)) {
        fprintf(stderr, "error: parameters for .print_code must be within code range %d to %d\n", CODE_START, CODE_START + CODE_SIZE);
        return;
    }
    else if (end_val < start_val) {
        fprintf(stderr, "error: second parameter of .%s must be >= first\n", messages[type]);
        return;
    }

    if (type) {

        /* print starting address if not divisible by 4, otherwise it's handled in loop */
        if (start_val % 4) printf("0x%.4x: ", (unsigned int) start_val);

        for (long l = start_val; l <= end_val; l++) {

            if ((l % 4) == 0) {
                printf("\n0x%.4x: ", (unsigned int) l);
            }

            print_bin(stack[l]);

            printf(" ");
        }
    }
    else {

        printf("\n");

        for (long l = start_val; l <= end_val; l++) {

            if (!code[l]) printf("0x%.4x: \n", (unsigned int) l);
            else printf("0x%.4x: %s\n", (unsigned int) l, code[l]);

        }

    }


    printf("\n");

}




void print_bin(char byte_val) {

    /* should ask if there's any way it's no null terminator here to not use the
     * whole extra byte for 9 instead of 8 and just print char by char totally safely in this small function
     */
    char buff[9];
    buff[8] = '\0';

    for (int i = 7; i >= 0; i--) {

        if (byte_val % 2) buff[i] = '1';
        else buff[i] = '0';

        byte_val /= 2;
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

    if (type == 2) {

        while (args && *args) *args++;
        if (args && *args) {
            fprintf(stderr, "error: .clear_all takes no parameters\n");
            return;
        }

        for (int i = CODE_START; i < CODE_START + CODE_SIZE; i++) {
            code[i] = NULL;
        }

        for (int i = STACK_START; i < STACK_START + STACK_SIZE; i++) {
            stack[i] = 0;
        }

    }

    else {

        char *mask = args;

        char *messages[2];
        messages[0] = "clear_code";
        messages[1] = "clear_stack";

        strsep(&mask, " ");

        if (!mask) {
            fprintf(stderr, "error: invalid number of parameters, proper form .%s [start address] [end address]\n", messages[type]);
            return;
        }

        char *endptr;

        errno = 0;
        long start_val = strtol(args, &endptr, 0);

        /* two error cases, not a number and number out of range of stack */
        if (errno || *endptr) {
            fprintf(stderr, "error: parameters for .%s must be numbers in base 10 or base 16 prepended with 0x\n", messages[type]);
            return;
        }
        else if (type && (start_val < STACK_START || start_val > STACK_START + STACK_SIZE)) {
            fprintf(stderr, "error: parameters for .clear_stack must be within stack range %d to %d\n", STACK_START, STACK_SIZE + STACK_START);
            return;
        }
        else if (!type && (start_val < CODE_START || start_val > CODE_START + CODE_SIZE)) {
            fprintf(stderr, "error: parameters for .clear_code must be within code range %d to %d\n", CODE_START, CODE_START + CODE_SIZE);
            return;
        }

        char *mask2 = mask;
        strsep(&mask2, " ");

        /* check for a third argument, trailing spaces allowed */
        if (mask2) {
            while(*mask2 == ' ') mask2++;
            if(*mask2) {
                fprintf(stderr, "error: invalid number of parameters, proper form .%s [start address] [end address]\n", messages[type]);
                return;
            }
        }

        errno = 0;
        long end_val = strtol(mask, &endptr, 0);

        /* same error cases as first arg, added case for arg less than first */
        if (errno || *endptr) {
            fprintf(stderr, "error: parameters for .%s must be numbers in base 10 or base 16 prepended with 0x\n", messages[type]);
            return;
        }
        else if (type && (end_val < STACK_START || end_val > STACK_START + STACK_SIZE)) {
            fprintf(stderr, "error: parameters for .clear_stack must be within stack range %d to %d\n", STACK_START, STACK_SIZE + STACK_START);
            return;
        }
        else if (!type && (end_val < CODE_START || end_val > CODE_START + CODE_SIZE)) {
            fprintf(stderr, "error: parameters for .clear_code must be within code range %d to %d\n", CODE_START, CODE_START + CODE_SIZE);
            return;
        }
        else if (end_val < start_val) {
            fprintf(stderr, "error: second parameter of .%s must be >= first\n", messages[type]);
            return;
        }

        if (type) {
            for (long l = start_val; l <= end_val; l++) {
                stack[l] = 0;
            }
        }
        else {
            for (long l = start_val; l <= end_val; l++) {
                code[l] = NULL;
            }
        }
    }
}

int run_instruction(char *instr) {

    char *mask = instr;

    strsep(&mask, " ");

    struct command *c = get_command(instr);

    if (!c) {
        fprintf(stderr, "error: instruction not found\n");
        return 1;
    }


}


