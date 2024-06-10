#include "instruction_set.h"
#include <string.h>
#include <stdlib.h>

#define MAX_STR 256

char *read_mem_region(char *s, int i) {

    if (!*s || !*(s + 1)) {
        fprintf(stderr, "error: incorrect format for system information file. please use template\n");
        exit(1);
    }

    char *mask = s + 1;
    
    while (*mask && *mask != ']') mask++;

    if (*mask == ']') {
        *mask == '\0';
    }
    else {
        fprintf(stderr, "error: incorrect format for system information file. please use template\n");
        exit(1);
    }

    char name[MAX_STR];
    int base;
    int bound;
    int direction;

    int check = sscanf(s + 1, "%s %d %d %d", name, &base, &bound, &direction);

    if (check != 4) {
        fprintf(stderr, "error: incorrect format for system information file. please use template\n");
        exit(1);
    }

    sys_info.mem_regions[i].name = strdup(name);
    sys_info.mem_regions[i].base = base;
    sys_info.mem_regions[i].bound = bound;
    sys_info.mem_regions[i].direction = direction;
    
    mask++;

    while (*mask && *mask != '[') mask++;

    if (*mask == '\0') return NULL;
    else return mask;

}

int parse_system_info(FILE *f) {


    char buff[MAX_STR];
    int i;
    int check;
    char *check2;

    check = fscanf(f, "%s", buff);

    if (check != 1 || !strcmp(buff, "SYSTEM\n")) {
        fprintf(stderr, "error: incorrect format for system information file. please use template\n");
        exit(1);
    }

    check = fscanf(f, "%s %d", buff, &i);

    if (check != 2 || !strcmp(buff, "register-count: ")) {
        fprintf(stderr, "error: incorrect format for system information file. please use template\n");
        exit(1);
    }

    sys_info.num_regs = i;
    sys_info.reg_names = malloc(sizeof (char *) * i);
    for (int x = 0; x < i; x++) {
        sys_info.reg_names[x] = NULL;
    }

    check = fscanf(f, "%s %d\n", buff, &i);

    if (check != 2 || !strcmp(buff, "memory-size: ")) {
        fprintf(stderr, "error: incorrect format for system information file. please use template\n");
        exit(1);
    }

    sys_info.mem_size = i;

    check2 = fgets(buff, MAX_STR, f);

    if (check2 == NULL) {
        fprintf(stderr, "error: incorrect format for system information file. please use template\n");
        exit(1);
    }

    char *c = &buff[0];

    while (*c && *c != '[') {
        c++;
    }

    if (*c == '\0') return 0;

    *(c - 1) = '\0';

    if (!strcmp(buff, "memory-regions: ")) {
        fprintf(stderr, "error: incorrect format for system information file. please use template\n");
        exit(1);
    }

    if (!*c || !*(c + 1)) {
        fprintf(stderr, "error: incorrect format for system information file. please use template\n");
        exit(1);
    }

    int iter = 0;

    char *current = read_mem_region(c, iter);

    while (current) {
        iter++;
        current = read_mem_region(current, iter);
    }

    sys_info.num_regions = iter + 1;


}
