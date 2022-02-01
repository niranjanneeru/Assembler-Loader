# include <stdio.h>
# include <stdlib.h>
#include <string.h>

typedef struct {
    char opcode[5];
    char hex[3];
} entry_op_tab;

typedef struct {
    char *symbol;
    long int addr;
} entry_sym_tab;

typedef struct {
    int i;
    entry_sym_tab symTab[50];
    int MAX;
} sym_tab;

entry_op_tab OP_TAB[26];
sym_tab SYM_TAB;
long int loc_ctr;
long int starting_addr;
long int program_size;

char *split(char *destination, const char *source, char delimiter, int begin) {
    while (source[begin] != delimiter) {
        *destination = source[begin];
        destination++;
        begin++;
    }
    *destination = '\0';
    return destination;
}

char *substr(char *destination, const char *source, unsigned int beg, int n) {
    while (n > 0) {
        *destination = *(source + beg);
        destination++;
        source++;
        n--;
    }
    *destination = '\0';
    return destination;
}

void load_op_tab() {
    FILE *fp = fopen("utility/op_tab.txt", "r");
    if (!fp) {
        perror("OP TABLE NOT FOUND: CONFIG ERROR\n");
        exit(EXIT_FAILURE);
    }
    char line[100];
    int i = 0;
    while (fgets(line, sizeof(line), fp)) {
        split(OP_TAB[i].opcode, line, ' ', 0);
        substr(OP_TAB[i].hex, line, strlen(line) - 3, 2);
//        printf("%s - %s\n", OP_TAB[i].opcode, OP_TAB[i].hex); // debug
        i++;
    }
    fclose(fp);
}

int search_op_tab(char *source) {
    for (int i = 0; i < 26; i++)
        if (strcmp(OP_TAB[i].opcode, source) == 0) return i;
    return -1;
}

int search_sym_tab(char *source) {
    for (int i = 0; i < SYM_TAB.MAX; i++) {
        if (!(SYM_TAB.symTab[i].symbol)) {
            break;
        }
        if (strcmp(SYM_TAB.symTab[i].symbol, source) == 0) {
            return i;
        }
    }
    return -1;
}

int insert_sym_tab(char *source) {
    if (search_sym_tab(source) != -1) {
        return -1;
    }
    SYM_TAB.symTab[SYM_TAB.i].symbol = malloc(strlen(source) * sizeof(char));
    strcpy(SYM_TAB.symTab[SYM_TAB.i].symbol, source);
    SYM_TAB.symTab[SYM_TAB.i].addr = loc_ctr;
    SYM_TAB.i++;
    return SYM_TAB.i - 1;
    // todo critical bug point
}