
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

/* tracks next available address in code array */
int code_index;

void run_user() {

    init_registers();

    printf("> ");

    char input_store[INPUT_SIZE];
    size_t size = sizeof(char) * INPUT_SIZE;

    char *input = &input_store[0];

    // for edge case testing (user enters line more than 256 bytes) add second pointer to spot, compare two.

    while(getline(&input, &size, stdin)) {

        while (*input == ' ') input++;

        input[strcspn(input, "\r\n")] = '\0';

        /* move past leading spaces */
        while(*input == ' ') input++;

        if (*input == '.') run_dot(input);

    }

}

void run_dot(char *input) {

    char *mask = input;
    strsep(&mask, " ");

    if (!strcmp(input, ".print_mem")) {
        if (!mask) {
            fprintf(stderr, "invalid number of parameters, should be:\n .print_mem [start address] [end address]\n");
            return;
        }
    }
    if (!strcmp(input, ".print_code")) {
        if (!mask) {
            fprintf(stderr, "invalid number of parameters, should be:\n .print_code [start address] [end address]\n");
            return;
        }
    }
    if (!strcmp(input, ".clear_mem")) {
        if (!mask) {
            fprintf(stderr, "invalid number of parameters, should be:\n .clear_mem [start address] [end address]\n");
            return;
        }
    }
    if (!strcmp(input, ".clear_code")) {
        if (!mask) {
            fprintf(stderr, "invalid number of parameters, should be:\n .print_mem [start address] [end address]\n");
            return;
        }
    }
    else {
        /* TODO: add "help" command, possibly with arguments for section and tell user to call it */
        fprintf(stderr, "invalid dot command\n");
    }
}


void init_registers() {

    AC = 0;
    BP = 0;
    SP = 0;
    PC = 0;
    IR = NULL;

}