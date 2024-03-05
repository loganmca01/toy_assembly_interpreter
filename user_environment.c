
#include "instruction_set.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* TODO: replace these all with globals and allow instruction set to set values for each */

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

/* tracks next available address in code array */
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
        fprintf(stderr, "invalid dot command\n");
    }
    return 0;
}

void run_print(int type, char *args) {

}

void run_clear(int type, char *args) {

}

