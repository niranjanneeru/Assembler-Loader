# include <stdlib.h>
# include <stdio.h>
# include <stdbool.h>
# include <string.h>
# include <math.h>
# include "utility.h"

typedef struct {
    char *label;
    char *opcode;
    char *operand;
} line_pass_1;

bool expectStart = true;
bool program_name = false;
bool expect_end = false;
FILE *intermediate_fp;


bool isOpCodeOrDirective(char *string);

void write_to_intermediate_file(line_pass_1 line);

void process(char *tokens[3], int c) {
    line_pass_1 line;
    if (isOpCodeOrDirective(tokens[0])) {
        // No Label Case
        line.label = NULL;
        line.opcode = tokens[0];
        // check for operand
        if (c == 2) line.operand = tokens[1];
        else line.operand = NULL;
    } else {
        // Label Found Case
        line.label = tokens[0];
        line.opcode = tokens[1];
        // todo handle error array index off bound
        // check for operand
        if (c == 3) line.operand = tokens[2];
        else line.operand = NULL;

    }
    if (strcmp(line.opcode, "START") == 0 && !line.operand) {
        line.operand = "0000";
    }
    if (strcmp(line.opcode, "END") == 0 && !line.operand) {
        expect_end = true;
    }
    write_to_intermediate_file(line);
//    for (int i = 0; i < c; i++) {
//        printf("%s ", tokens[i]);
//    } // debug
}

void write_to_intermediate_file(line_pass_1 line) {
    printf("%s %s %s\n", line.label, line.opcode, line.operand); // debug
    if (line.label) {
        if (insert_sym_tab(line.label) == -1) {
            perror("Already Existing Label");
        }
    }
    if (strcmp(line.opcode, "START") == 0) {
        expectStart = false;
        if (line.operand) {
            starting_addr = strtol(line.operand, NULL, 16);
            loc_ctr = starting_addr;
        } else {
            starting_addr = loc_ctr = 0;
        }
        program_name = line.operand != NULL;

        fprintf(intermediate_fp, "%lX START %s\n", loc_ctr, line.operand);
        return;
    } else if (expectStart) {
        starting_addr = loc_ctr = 0;
        fprintf(intermediate_fp, "%lX START 0000\n", loc_ctr);
        expectStart = false;
    }

    int loc = search_op_tab(line.opcode);
    if (loc != -1) {
        // OP CODE Found
        if (strcmp(line.opcode, "RSUB") == 0) {
            fprintf(intermediate_fp, "%lX RSUB 0000\n", loc_ctr);
        } else {
            fprintf(intermediate_fp, "%lX %s %s\n", loc_ctr, line.opcode, line.operand);
        }
        loc_ctr += 3;
    } else if (strcmp(line.opcode, "WORD") == 0) {
        fprintf(intermediate_fp, "%lX WORD %s\n", loc_ctr, line.operand);
        loc_ctr += 3;
    } else if (strcmp(line.opcode, "RESW") == 0) {
        fprintf(intermediate_fp, "%lX RESW %s\n", loc_ctr, line.operand);
        loc_ctr += 3 * strtol(line.operand, NULL, 10);
    } else if (strcmp(line.opcode, "RESB") == 0) {
        fprintf(intermediate_fp, "%lX RESB %s\n", loc_ctr, line.operand);
        loc_ctr += strtol(line.operand, NULL, 10);
    } else if (strcmp(line.opcode, "BYTE") == 0) {
        fprintf(intermediate_fp, "%lX BYTE %s\n", loc_ctr, line.operand);
        loc_ctr += (line.operand[0] == 'C') ? strlen(line.operand) - 3 : ceil(
                (strlen(line.operand) - 3) / 2.0);
    } else if (strcmp(line.opcode, "END") == 0) {
        program_size = loc_ctr - starting_addr;
        if (expect_end) {
            fprintf(intermediate_fp, "%lX END %04lX\n", loc_ctr, starting_addr);
        } else {
            fprintf(intermediate_fp, "%lX END %s\n", loc_ctr, line.operand);
        }
    } else {
        perror("Invalid OP Code");
        printf("\n%s", line.opcode);
    }

}

bool isOpCodeOrDirective(char *string) {
    if (search_op_tab(string) != -1) {
        return true;
    }
    if (strcmp(string, "WORD") == 0) {
        return true;
    }
    if (strcmp(string, "BYTE") == 0) {
        return true;
    }
    if (strcmp(string, "RESW") == 0) {
        return true;
    }
    if (strcmp(string, "RESB") == 0) {
        return true;
    }
    if (strcmp(string, "START") == 0) {
        return true;
    }
    if (strcmp(string, "END") == 0) {
        return true;
    }
    return false;
}


void pass1(const char *filename) {
    SYM_TAB.i = 0;
    SYM_TAB.MAX = 50;
    intermediate_fp = fopen("intermediate.out", "w");
    if (intermediate_fp == NULL) {
        perror("Unable to Create Intermediate File\n");
        exit(EXIT_FAILURE);
    }
    FILE *fp;
    char line[100];
    fp = fopen(filename, "r");

    // check for input file in the location
    if (fp == NULL) {
        perror("Unable to Locate the File\n");
        exit(EXIT_FAILURE);
    }

    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '.' || line[0] == '\n')continue; // skip comments
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        char *tokens[3];
        int c = 0;
        char *token = strtok(line, " ");
        while (token != NULL && c != 3) {
            tokens[c++] = token;
            token = strtok(NULL, " ");
        }
        process(tokens, c);
    }
    fclose(fp);
    fclose(intermediate_fp);

    //debug
//    FILE *sym_fp;
//    sym_fp = fopen("utility/sym_file.txt", "w");
//    for (int i = 0; i < SYM_TAB.MAX; i++) {
//        if (!(SYM_TAB.symTab[i].symbol)) {
//            break;
//        }
//        fprintf(sym_fp, "%s - %lX - %ld\n", SYM_TAB.symTab[i].symbol, SYM_TAB.symTab[i].addr, SYM_TAB.symTab[i].addr);
//    }
//    fclose(sym_fp);
}