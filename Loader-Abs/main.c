# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include "utility.h"

long long int pc = 0;
long long int starting_address = 0;
long long int execution_address = 0;
long long int length = 0;

void manage_header_record(char line[100]);

void manage_text_record(char line[100]);

void manage_end_record(char line[100]);

FILE *out;

int main(int argc, char **argv) {

    char line[100];

    if (argc == 1) {
        perror("No Input File Found\n");
        exit(EXIT_FAILURE);
    }

    char *filename = argv[1];

    FILE *inp;
    inp = fopen(filename, "r");

    if (inp == NULL) {
        perror("Unable to Open Object Code File\n");
        exit(EXIT_FAILURE);
    }

    out = fopen("output.txt", "w");

    while (fgets(line, sizeof(line), inp)) {
        if (line[0] == '\n')continue; // skip new line
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        if (line[0] == 'H') {
            manage_header_record(line);
        } else if (line[0] == 'E') {
            manage_end_record(line);
        } else if (line[0] == 'T') {
            manage_text_record(line);
        } else {
            perror("Corrupted File");
            fclose(inp);
            exit(EXIT_FAILURE);
        }
    }
    fclose(inp);
    return 0;
}

void manage_header_record(char line[100]) {
    char addr[7];
    substr(addr, line, 7, 6);
    pc = strtol(addr, NULL, 16);
    starting_address = pc;
    substr(addr, line, 13, 6);
    length = strtol(addr, NULL, 16);
}

void manage_text_record(char line[100]) {
    char addr[7];
    substr(addr, line, 1, 6);
    long long int record_addr = strtol(addr, NULL, 16);
    char record_len[3];
    substr(record_len, line, 7, 2);
    int len = strtol(record_len, NULL, 16);
    while (pc < record_addr) {
        fprintf(out, "%llX XX\n", pc);
        pc++;
    }
//    int flag = 0;
    for (int i = 9; i < strlen(line); i += 2, pc++) {
//        flag = 0;
//        if (line[i + 1] == '\0') {
//            flag = 1;
//            line[i + 1] = 'X';
//        }
        fprintf(out, "%llX %c%c\n", pc, line[i], line[i + 1]);
//        if (flag) {
//            line[i + 1] = '\0';
//        }
    }
}

void manage_end_record(char line[100]) {
    char addr[7];
    substr(addr, line, 1, 6);
    execution_address = strtol(addr, NULL, 16);
    printf("Start Address %llX\n", starting_address);
    printf("Start Execution From %llX\n", execution_address);
    printf("Program Length %llX\n", length);
    printf("Program Counter %llX\n", pc);
}

