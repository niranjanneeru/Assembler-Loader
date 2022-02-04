# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include "utility.h"

long long int pc = 0;
long long int starting_address = 0;
long long int execution_address = 0;
long long int length = 0;

typedef struct {
    long long int addr;
    int ascii;
} element;

element *arr;

void manage_header_record(char line[100]);

void manage_text_record(char line[100]);

void manage_modification_record(char line[100]);

void manage_end_record(char line[100]);

void modify(long long int temp, long long int i, int i1, long long int address);

FILE *out;

int main(int argc, char **argv) {

    char line[100];

    if (argc == 1) {
        perror("No Input File Found\n");
        exit(EXIT_FAILURE);
    }

    printf("Enter Starting Address: ");
    scanf("%llX", &starting_address);

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
        } else if (line[0] == 'M') {
            manage_modification_record(line);
        } else {
            perror("Corrupted File");
            fclose(inp);
            exit(EXIT_FAILURE);
        }
    }
    fclose(inp);
    fclose(out);
    free(arr);
    return 0;
}

void manage_header_record(char line[100]) {
    char addr[7];
    pc = 0;
    substr(addr, line, 13, 6);
    length = strtol(addr, NULL, 16);
    arr = (element *) malloc(length * sizeof(element));
}

void manage_text_record(char line[100]) {
    char addr[7];
    substr(addr, line, 1, 6);
    long long int record_addr = strtol(addr, NULL, 16);
    while (pc < record_addr) {
        arr[pc].addr = starting_address + pc;
        arr[pc].ascii = -1;
        pc++;
    }
    for (int i = 9; i < strlen(line); i += 2, pc++) {
        arr[pc].addr = starting_address + pc;
        char temp[3];
        temp[0] = line[i];
        temp[1] = line[i + 1];
        temp[2] = '\0';
        arr[pc].ascii = strtol(temp, NULL, 16);
    }
}

void manage_modification_record(char line[100]) {
    char addr[7];
    substr(addr, line, 1, 6);
    long long int temp = strtol(addr, NULL, 16);
    modify(temp, temp + starting_address, 5, starting_address);
}

void modify(long long int temp, long long int i, int i1, long long int address) {
    address += 0x100000;
    for (int j = (int) temp + 2; j != temp; j--) {
        arr[j].ascii += (int) address % 0x100;
        address /= 0x100;
    }
}

void manage_end_record(char line[100]) {
    char addr[7];
    substr(addr, line, 1, 6);
    execution_address = starting_address + strtol(addr, NULL, 16);
    printf("Start Address %llX\n", starting_address);
    printf("Start Execution From %llX\n", execution_address);
    printf("Program Length %llX\n", length);
    printf("Program Counter %llX\n", pc);
    for (int i = 0; i < length; i++) {
        if (arr[i].ascii == -1)
            fprintf(out, "%05llX XX\n", arr[i].addr);
        else
            fprintf(out, "%05llX %02X\n", arr[i].addr, arr[i].ascii);
    }
}

