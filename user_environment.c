
#include "instruction_set.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

/* TODO: replace these all with globals (possibly global system_info struct) and allow instruction set to set values for each */

/* default 8 KB stack */
unsigned int STACK_SIZE;
unsigned int STACK_START;

/* default 2 KB code (each instruction treated as 1 byte in simulation) */
unsigned int CODE_SIZE;
unsigned int CODE_START;

#define INPUT_SIZE 256
#define REG_BUFF_SIZE 16

uint8_t *stack;
char **code;

/* TODO: when adding custom registers to instruction set, allow for different sizes (and possibly different word size)
 * TODO ALSO: maybe assign all registers numbers and add conversion, then check for correct reg size array by number
 *
 * TODO IMPORTANT: get special purpose register table from other file, this one should only be gen purpose
 */

/* tracks next available address in code array - not always same as PC in case of jumps */
int pc_loc;

void run_user() {

    print_welcome();

    /* eventually will be set based on where PC spec reg is in system info from original parse */
    pc_loc = 1;

    printf("> ");

    char input_store[INPUT_SIZE];
    size_t size = sizeof(char) * INPUT_SIZE;

    char *input = &input_store[0];

    // for edge case error catching (user enters line more than 256 bytes) add second pointer to spot, compare two.

    int done = 0;

    while(getline(&input, &size, stdin)) {

        input[strcspn(input, "\r\n")] = '\0';

        /* move past leading spaces */
        while(*input == ' ') input++;

        char *full_input = strdup(input);

        if (*input == '.') {
            done = run_dot(input);
            free(full_input);
        }
        else {
            if (spec_reg[pc_loc].value < CODE_START || spec_reg[pc_loc].value > (CODE_START + CODE_SIZE)) {
                fprintf(stderr, "error: PC pointing at address outside of code\n");
                /* might be better to return (or goto?) here, cause a similar error while running instructions will probably need to return (or use goto?) instead of break */
                goto end_of_loops;
            }
            code[spec_reg[pc_loc].value] = full_input;
        }

        if (done) break;

        while(code[spec_reg[pc_loc].value]) {

            if((spec_reg[pc_loc].value < CODE_START) || (spec_reg[pc_loc].value > (CODE_START + CODE_SIZE))) {
                fprintf(stderr, "error: PC pointing at address outside of code\n");
                /* need to break out of both loops here */
                goto end_of_loops;
            }

            char *tmp = strdup(code[spec_reg[pc_loc].value]);
            int tmpi = spec_reg[pc_loc].value;

            int check = 0;

            if (tmp[0] == '_') {
                check = run_underscore(tmp);
                if (check != 2) spec_reg[pc_loc].value++;
            }
            else {
                int check = run_instruction(tmp);
                /* increment only if PC isn't changed */
                if (tmpi == spec_reg[pc_loc].value) spec_reg[pc_loc].value++;
            }

            free(tmp);

            if (check == 1) {
                fprintf(stderr, "error thrown by code address %d from instruction: %s\n", tmpi, code[tmpi]);
                goto end_of_loops;
            }

        }

        printf("> ");

    }

    end_of_loops:;

}

void print_welcome() {

    printf("Welcome to the toy assembly interpreter.\n");
    print_system_info();

    printf("For information about the system commands available to you use .help\n\n");

    printf("Keep in mind that dot commands are not loaded into code memory, and should\n"
           "be ignored when calculating instruction locations for jumps.\n\n");

    printf("Use .quit to quit the program\n\n\n");

}

void print_spec_regs() {

    for (int i = 0; i < NUM_SPEC_REG; i++) {
        printf("%s", spec_reg[i].name);
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
    printf(".clear_stack [start address] [end address]    clear the contents of the stack between two addresses\n\n");
    printf(".clear_code [start address] [end address]     clear the contents of code memory between two addresses\n\n");
    printf(".clear_all                                    clear everything from stack and code memory\n\n");
    printf(".file [filepath]                              load commands from file into next available code address, only need file name if in same folder\n\n");

    printf("_print_stack [start address] [end address]    print the contents of stack between two addresses\n\n");
    printf("_print_code [start address] [end address]     print the contents of code memory between two addresses\n\n");
    printf("_exit                                         exit current instructions, functions the same as jumping PC to after last instruction\n\n");

    printf("For language specific commands look in text document passed to interpreter\n\n\n");

}

int run_dot(char *input) {

    char *mask = input;
    strsep(&mask, " ");

    if (!strcmp(input, ".clear_stack")) {
        while (mask && *mask == ' ') mask++;
        run_clear(1, mask);
    }
    else if (!strcmp(input, ".clear_code")) {
        while (mask && *mask == ' ') mask++;
        run_clear(0, mask);
    }
    else if (!strcmp(input, ".clear_regs")) {
        while (mask && *mask == ' ') mask++;
        run_clear(2, mask);
    }
    else if (!strcmp(input, ".clear_all")) {
        while (mask && *mask == ' ') mask++;
        run_clear(3, mask);
    }
    else if (!strcmp(input, ".help")) {
        print_help_message();
    }
    else if (!strcmp(input, ".file")) {
        load_file(mask);
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

int run_underscore(char *input) {

    char *mask = input;
    strsep(&mask, " ");

    if (!strcmp(input, "_print_stack")) {
        while (mask && *mask == ' ') mask++;
        return run_print(1, mask);
    }
    else if (!strcmp(input, "_print_code")) {
        while (mask && *mask == ' ') mask++;
        return run_print(0, mask);
    }
    else if (!strcmp(input, "_print_regs")) {
        while (mask && *mask == ' ') mask++;
        return run_print(2, mask);
    }
    else if (!strcmp(input, "_exit")) {
        while(code[spec_reg[pc_loc].value]) {
            spec_reg[pc_loc].value++;
        }
        return 2;
    }

    return 0;

}

/*
 * Print contents of stack or code memory
 * Called by run_dot
 *
 * mode 0 = code
 *      1 = stack
 *      2 = regs
 *
 * function is long but most of it is just checking error cases
 *
 */
int run_print(int type, char *args) {

    char *mask = args;

    if (type == 2) {

        if (mask) {
            while(*mask == ' ') mask++;
            if(*mask) {
                fprintf(stderr, "error: .print_regs requires no parameters\n");
                return 1;
            }
        }

        for (int i = 0; i < NUM_SPEC_REG; i++) {
            printf("%s = %d\n", spec_reg[i].name, spec_reg[i].value);
        }
        for (int i = 0; i < NUM_GEN_REG; i++) {
            printf("%s = %d\n", gen_reg[i].name, gen_reg[i].value);
        }

        return 0;

    }


    char *messages[2];
    messages[0] = "print_code";
    messages[1] = "print_stack";

    strsep(&mask, " ");

    if (!mask) {
        fprintf(stderr, "error: invalid number of parameters, proper form .%s [start address] [end address]\n", messages[type]);
        return 1;
    }

    char *endptr;

    errno = 0;
    long start_val = strtol(args, &endptr, 0);

    /* two error cases, not a number and number out of range of stack */
    if (errno || *endptr) {
        fprintf(stderr, "error: parameters for .%s must be numbers in base 10 or base 16 prepended with 0x\n", messages[type]);
        return 1;
    }
    else if (type && (start_val < STACK_START || start_val > STACK_START + STACK_SIZE)) {
        fprintf(stderr, "error: parameters for .print_stack must be within stack range %d to %d\n", STACK_START, STACK_SIZE + STACK_START);
        return 1;
    }
    else if (!type && (start_val < CODE_START || start_val > CODE_START + CODE_SIZE)) {
        fprintf(stderr, "error: parameters for .print_code must be within code range %d to %d\n", CODE_START, CODE_START + CODE_SIZE);
        return 1;
    }

    char *mask2 = mask;
    strsep(&mask2, " ");

    /* check for a third argument, trailing spaces allowed */
    if (mask2) {
        while(*mask2 == ' ') mask2++;
        if(*mask2) {
            fprintf(stderr, "error: invalid number of parameters, proper form .%s [start address] [end address]\n", messages[type]);
            return 1;
        }
    }

    errno = 0;
    long end_val = strtol(mask, &endptr, 0);

    /* same error cases as first arg, added case for arg less than first */
    if (errno || *endptr) {
        fprintf(stderr, "error: parameters for .%s must be numbers in base 10 or base 16 prepended with 0x\n", messages[type]);
        return 1;
    }
    else if (type && (end_val < STACK_START || end_val > STACK_START + STACK_SIZE)) {
        fprintf(stderr, "error: parameters for .print_stack must be within stack range %d to %d\n", STACK_START, STACK_SIZE + STACK_START);
        return 1;
    }
    else if (!type && (end_val < CODE_START || end_val > CODE_START + CODE_SIZE)) {
        fprintf(stderr, "error: parameters for .print_code must be within code range %d to %d\n", CODE_START, CODE_START + CODE_SIZE);
        return 1;
    }
    else if (end_val < start_val) {
        fprintf(stderr, "error: second parameter of .%s must be >= first\n", messages[type]);
        return 1;
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
    return 0;

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
 *      2 = clear regs
 *      3 = clear all of stack, code, and regs
 *
 */
void run_clear(int type, char *args) {

    if (type == 3) {

        while (args && *args) args++;
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

        for (int i = 0; i < NUM_SPEC_REG; i++) {
            spec_reg[i].value = 0;
        }

        for (int i = 0; i < NUM_GEN_REG; i++) {
            gen_reg[i].value = 0;
        }

    }
    else if (type == 2) {
        for (int i = 0; i < NUM_SPEC_REG; i++) {
            spec_reg[i].value = 0;
        }

        for (int i = 0; i < NUM_GEN_REG; i++) {
            gen_reg[i].value = 0;
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

void load_file(char *filepath) {

    char *mask = filepath;

    strsep(&mask, " ");

    while (mask && *mask == ' ') mask++;

    if (mask && *mask) {
        fprintf(stderr, "error: only one argument expected for .file\n");
        return;
    }

    FILE *source = fopen(filepath, "r");

    if (!source) {
        fprintf(stderr, "error: file not found\n");
        return;
    }

    char input_store[INPUT_SIZE];
    size_t size = sizeof(char) * INPUT_SIZE;

    char *input = &input_store[0];

    // for edge case testing (user enters line more than 256 bytes) add second pointer to spot, compare two.

    int done = 0;

    int cnt = spec_reg[pc_loc].value;

    while(getline(&input, &size, source)) {

        int eof = 0;

        if (input[strlen(input) - 1] != '\n') eof = 1;
        while(*input == ' ') input++;

        input[strcspn(input, "\r\n")] = '\0';

        if (!input || !*input) {
            break;
        }

        if (input[0] == '.') {
            fprintf(stderr, "error: attempting to load dot command into memory from file, only instructions and _ commands allowed in file input\n");
            return;
        }

        if((cnt < CODE_START) || (cnt > (CODE_START + CODE_SIZE))) {
            fprintf(stderr, "error: attempting to load instruction to location outside code memory (PC is outside range)\n");
            return;
        }

        code[cnt++] = strdup(input);

        if (eof) break;

    }

}


int run_instruction(char *instr) {

    struct sym_map reg_map[REG_BUFF_SIZE];
    int num_reg_args = 0;

    char *mask = instr;
    strsep(&mask, " ");

    struct command *c = get_command(instr);

    if (!c) {
        fprintf(stderr, "error: instruction not found\n");
        return 1;
    }

    struct sym_list *sl = c->args;
    struct sym_list *list_iter;

    for (list_iter = sl; list_iter != NULL; list_iter = list_iter->next) {

        while (mask && *mask == ' ') mask++;

        char *mask2 = mask;
        strsep(&mask2, " ");

        if (!mask || !*mask) {
            fprintf(stderr, "error: not enough arguments for instruction %s\n", instr);
            return 1;
        }

        int check = 0;

        if (list_iter->sym->type == 1) {
            char *endptr;

            errno = 0;
            long var_val = strtol(mask, &endptr, 0);

            if (errno || *endptr) {
                fprintf(stderr, "error: argument for variable type instruction parameter must be numbers in base 10 or base 16 prepended with 0x\n");
                return 1;
            }

            list_iter->sym->value = var_val;
            check = 1;
        }

        if (!check) {
            for (int i = 0; i < NUM_SPEC_REG; i++) {
                if (!strcmp(spec_reg[i].name, mask)) {
                    list_iter->sym->value = spec_reg[i].value;
                    check = 1;
                    reg_map[num_reg_args].dummy = list_iter->sym;
                    reg_map[num_reg_args].real = &spec_reg[i];
                    num_reg_args++;
                }
            }
        }
        if (!check) {
            for (int i = 0; i < NUM_GEN_REG; i++) {
                if (!strcmp(gen_reg[i].name, mask)) {
                    list_iter->sym->value = gen_reg[i].value;
                    check = 1;
                    reg_map[num_reg_args].dummy = list_iter->sym;
                    reg_map[num_reg_args].real = &gen_reg[i];
                    num_reg_args++;
                }
            }
        }

        if (!check) {
            fprintf(stderr, "error: invalid argument %s\n", mask);
            return 1;
        }

        mask = mask2;
    }

    while (mask && *mask == ' ') mask++;
    if (mask) {
        fprintf(stderr, "error: too many arguments for instruction %s\n", instr);
        return 1;
    }

    int err = 0;

    /* null terminate reg array, check that dummy isn't null to continue */
    reg_map[num_reg_args].dummy = NULL;

    for (struct ast_list *al = c->actions; al != NULL; al = al->next) {
        err = run_action(al->a, &reg_map[0]);
        if (err) break;
    }

    return err;

}

int run_action(struct ast *a, struct sym_map *reg_map) {

    /* if there's a conditional in the action, evaluate it first then either return or continue with the rest */
    if (a->nodetype == 'i') {
        int cond = eval_cmp(((struct flow *)a)->cond);
        if (!cond) return 0;
        else a = ((struct flow *)a)->then;
    }

    /* This should be entirely impossible, but added here because if
     * it does happen it would be tough to tell why it breaks without this check
     */
    if (a->nodetype != '=') {
        fprintf(stderr, "error: wrong node type for root of action\n");
        return 1;
    }

    struct ast *left = a->l;
    struct ast *right = a->r;

    if (left->nodetype == 'm') {
        int val = eval_ast(((struct memref *) left)->loc);
        if (val < STACK_START || val > STACK_START + STACK_SIZE) {
            fprintf(stderr, "error: attempted to access memory outside of stack range\n");
            return 1;
        }
        /* TODO NOTE: not sure how strict I should be about this, maybe two different modes given in system info? */
        else if (val % 4) {
            fprintf(stderr, "error: alignment issue, 4 byte register values should be loaded into memory at locations divisible by 4\n");
            return 1;
        }

        /* TODO: figure out how little/big endian affects this and make it portable */
        int tmp = eval_ast(right);

        for (int i = 0; i < 4; i++) {
            stack[val + i] = (tmp >> (8 * (3 - i))) & 0xff;
        }

        return 0;
    }
    else if (left->nodetype == 'r') {

        for (int i = 0; i < NUM_SPEC_REG; i++) {
            if (!strcmp(spec_reg[i].name, ((struct symref *)left)->sym->name)) {
                spec_reg[i].value = eval_ast(right);
                /* TODO: add error case where eval fails/returns error code */
                return 0;
            }
        }

        for (int i = 0; i < REG_BUFF_SIZE; i++) {
            if (!reg_map[i].dummy) break;

            if (((struct symref *)left)->sym == reg_map[i].dummy) {

                reg_map[i].real->value = eval_ast(right);
                /* TODO: add error case where eval fails/returns error code */
                return 0;
            }
        }

        /* should be caught at instruction set level, but less confident about this one being impossible */
        fprintf(stderr, "error: register being assigned to not found\n");
        return 1;
    }
    /* Again this error should be caught at instruction set level,
     * but if it somehow happens I want to be prepared.
     */
    else {
        fprintf(stderr, "error: attempting to assign value to something other than register or memory location\n");
        return 1;
    }

}


