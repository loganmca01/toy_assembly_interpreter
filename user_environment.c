
#include "instruction_set.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* 8 KB stack */
#define STACK_SIZE 8192

/* 2 KB code (each instruction treated as 1 byte in simulation) */
#define CODE_SIZE 2048

#define INPUT_SIZE 256

char stack[STACK_SIZE];
char *code[CODE_SIZE];

int AC;
int BP;
int SP;
int PC;
char *IR;

/* general purpose registers */
int gen_reg[8];

/* tracks next available address in code array */
int code_index;

void run_user() {

    init_registers();

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

    }




}

void print_welcome() {

    printf("Welcome to the toy assembly interpreter.\n"
           "5 Special purpose registers AC, BP, SP, PC, IR\n"
           "8 General purpose registers reg0 - reg7\n\n"
           "The stack is 8 KB, or 8192 bytes. Code is 2 KB or 2048 bytes.\n"
           "For information about the system and the dot commands available to you use .help\n\n"
           "Keep in mind that dot commands are not loaded into code memory, and should\n"
           "be ignored when calculating instruction locations for jumps.\n\n");
}


int run_dot(char *input) {

    char *mask = input;
    strsep(&mask, " ");

    if (!strcmp(input, ".print_mem")) {
        while (mask && *mask == ' ') mask++;
    }
    else if (!strcmp(input, ".print_code")) {
        while (mask && *mask == ' ') mask++;
    }
    else if (!strcmp(input, ".clear_mem")) {
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

    }
    else if (!strcmp(input, ".quit")) {
        return 1;
    }
    else {
        /* TODO: add "help" command, possibly with arguments for section and tell user to call it */
        fprintf(stderr, "invalid dot command\n");
    }
}

void run_print(int type, char *args) {

}

void run_clear(int type, char *args) {

}

void init_registers() {

    AC = 0;
    BP = 0;
    SP = 0;
    PC = 0;
    IR = NULL;

}