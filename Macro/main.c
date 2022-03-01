#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *label;
    char *opcode;
    char *operand;
    char *inp;
} line;

line *getLine(bool);

FILE *out_fp;

void processForMacro(line *data, bool expanding);

void define(line *data, bool expanding);

int count(char *operand);

void expand(line *data, bool expanding, int i);

int currentExpanded = 0;

int getCurrentExpanding() {
    return currentExpanded;
}

void setCurrentExpanding(int loc) {
    currentExpanded = loc;
}

typedef struct {
    char opcode[5];
    char hex[3];
} entry_op_tab;


typedef struct {
    char *name;
    line *data[100];
    int index;
    int read;
    char **args;
    int arg_count;
} NAM_TAB_TEMPLATE;
NAM_TAB_TEMPLATE NAMTAB[100];
int nam_tab_entry = 0;

int search_name_tab(const char *arr) {
    if (!arr) return 0;
    for (int i = 0; i < nam_tab_entry; i++) {
        if (strcmp(NAMTAB[i].name, arr) == 0) {
            return i + 1;
        }
    }
    return 0;
}

entry_op_tab OP_TAB[26];

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

char *replaceWord(const char *s, const char *oldW,
                  const char *newW) {
    char *result;
    int i, cnt = 0;
    int newWlen = strlen(newW);
    int oldWlen = strlen(oldW);

    // Counting the number of times old word
    // occur in the string
    for (i = 0; s[i] != '\0'; i++) {
        if (strstr(&s[i], oldW) == &s[i]) {
            cnt++;

            // Jumping to index after the old word.
            i += oldWlen - 1;
        }
    }

    // Making new string of enough length
    result = (char *) malloc(i + cnt * (newWlen - oldWlen) + 1);

    i = 0;
    while (*s) {
        // compare the substring with the result
        if (strstr(s, oldW) == s) {
            strcpy(&result[i], newW);
            i += newWlen;
            s += oldWlen;
        } else
            result[i++] = *s++;
    }

    result[i] = '\0';
    return result;
}

void load_op_tab() {
    FILE *fp = fopen("D:\\Neeru\\PROJECTS\\Assemblers and Loaders\\Macro\\op_tab.txt", "r");
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

FILE *input_fp;


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
    if (strcmp(string, "MACRO") == 0) {
        return true;
    }
    if (strcmp(string, "MEND") == 0) {
        return true;
    }
    return false;
}

line *process(char *tokens[3], int c) {
    line *data = (line *) malloc(sizeof(line));
//    for (int i = 0; i < c; i++) {
//        printf("%s ", tokens[i]);
//    }
    if (isOpCodeOrDirective(tokens[0])) {
        // No Label Case
        data->label = NULL;
        data->opcode = tokens[0];
        // check for operand
        if (c == 2) data->operand = tokens[1];
        else data->operand = NULL;
    } else {
        // Label Found Case
        data->label = tokens[0];
        data->opcode = tokens[1];
        // todo handle error array index off bound
        // check for operand
        if (c == 3) data->operand = tokens[2];
        else data->operand = NULL;

    }
//    write_to_intermediate_file(line);
//    for (int i = 0; i < c; i++) {
//        printf("%s ", tokens[i]);
//        break;
//    } // debug

    return data;
}

line *getLine(bool expanding) {
    if (!expanding) {
        char *arr = malloc(sizeof(char) * 100);
        fgets(arr, 100, input_fp);
        if (arr[0] == '.' || arr[0] == '\n')return NULL; // skip comments
        if (arr[strlen(arr) - 1] == '\n') {
            arr[strlen(arr) - 1] = '\0';
        }
        char *temp = malloc(sizeof(char) * 100);
        strcpy(temp, arr);
        char *tokens[3];
        int c = 0;
        char *token = strtok(arr, " ");
        while (token != NULL && c != 3) {
            tokens[c++] = token;
            token = strtok(NULL, " ");
        }
//        for (int i = 0; i < c; i++) {
//            printf("%s ", tokens[i]);
//        }
        line *data = process(tokens, c);
        data->inp = temp;
        return data;
    } else {
//        NAM_TAB_TEMPLATE *tem = &;
        int loc = getCurrentExpanding();
        if (NAMTAB[loc].read >= NAMTAB[loc].index) {
            NAMTAB[loc].read = 0;
        }
        line *line_temp = NAMTAB[loc].data[NAMTAB[loc].read++];
        char *temp = strdup(line_temp->operand);
        if (temp) {
            for (int i = 0; i < NAMTAB[loc].arg_count; i++) {
                char pos_notation[3];
                sprintf(pos_notation, "?%d", i + 1);
                if (strcmp(pos_notation, temp) == 0) {
                    free(temp);
                    temp = strdup(NAMTAB[loc].args[i]);
                    break;
                }
//                printf("%s %s %s \n", temp, pos_notation, NAMTAB[loc].args[i]);
            }
        } else {
            return line_temp;
        }
        line *new_temp = (line *) malloc(sizeof(line));
        new_temp->label = line_temp->label;
        new_temp->opcode = line_temp->opcode;
        new_temp->operand = temp;
        return new_temp;
    }
}

int main(int argc, char **argv) {
    load_op_tab();
    if (argc == 1) {
        perror("Argument Missing\n");
        exit(0);
    }
    input_fp = fopen(argv[1], "r");
    if (!input_fp) {
        perror("FILE NOT FOUND: CONFIG ERROR\n");
        exit(EXIT_FAILURE);
    }
    out_fp = fopen("expanded.txt", "w");
    if (!out_fp) {
        perror("FILE NOT FOUND: CONFIG ERROR\n");
        exit(EXIT_FAILURE);
    }
    bool expanding = false;
    line *data = getLine(expanding);
    while (strcmp(data->opcode, "END") != 0) {
//        printf("%s %s %s\n", data->label, data->opcode, data->operand);
        data = getLine(expanding);
        processForMacro(data, expanding);
    }
    fclose(input_fp);
    free(data);
    return 0;
}

void processForMacro(line *data, bool expanding) {
    int found = search_name_tab(data->label);
    if (found) {
        expand(data, expanding, found - 1);
    } else if (strcmp(data->opcode, "MACRO") == 0) {
        define(data, expanding);
    } else {
        fprintf(out_fp, "%s %s %s\n", data->label, data->opcode, data->operand);
    }
}

void expand(line *data, bool expanding, int i) {
    expanding = true;
    char *ptr = strtok(data->opcode, ",");
    int j = 0;
    while (ptr) {
        NAMTAB[i].args[j] = ptr;
        ptr = strtok(NULL, ",");
        j++;
    }
//    printf("%d %s %s %s %s\n", i, NAMTAB[i].args[0], NAMTAB[i].args[1], NAMTAB[i].args[2], NAMTAB[i].args[3]);
    fprintf(out_fp, ". %s\n", data->inp);
    setCurrentExpanding(i);
    data = getLine(expanding);
    while (strcmp(data->opcode, "MEND") != 0) {
        data = getLine(expanding);
        processForMacro(data, expanding);
    }
    expanding = false;
}


void define(line *data, bool expanding) {
    NAMTAB[nam_tab_entry].name = strdup(data->label);
    NAMTAB[nam_tab_entry].index = 0;
    NAMTAB[nam_tab_entry].read = 0;
//    printf("%s %s %s\n%s\n",data->label,data->opcode,data->operand,data->inp);
    int num_of_elem = count(data->operand);
    NAMTAB[nam_tab_entry].args = (char **) malloc(sizeof(char *) * num_of_elem);
    char *temp = strdup(data->operand);
    char *ptr = strtok(temp, ",");
    NAMTAB[nam_tab_entry].arg_count = 0;
    while (ptr) {
        NAMTAB[nam_tab_entry].args[NAMTAB[nam_tab_entry].arg_count] = ptr;
        ptr = strtok(NULL, ",");
        NAMTAB[nam_tab_entry].arg_count++;
    }
    NAMTAB[nam_tab_entry].data[NAMTAB[nam_tab_entry].index++] = data;
//    printf("%s %s %s\n", indata->label, indata->opcode, indata->operand);
    int level = 1;
    while (level) {
        line *indata = getLine(expanding);
        if (indata->operand) {
            for (int i = 0; i < NAMTAB[nam_tab_entry].arg_count; i++) {
                char pos_notation[3];
                sprintf(pos_notation, "?%d", i + 1);
                indata->operand = replaceWord(indata->operand, NAMTAB[nam_tab_entry].args[i], pos_notation);
            }
        }
        NAMTAB[nam_tab_entry].data[NAMTAB[nam_tab_entry].index++] = indata;
        if (strcmp(indata->opcode, "MACRO") == 0)level++;
        if (strcmp(indata->opcode, "MEND") == 0)level--;
    }
    nam_tab_entry++;
}

int count(char *operand) {
    int c = 1;
    while (*operand != '\0') {
        if (*operand == ',') c++;
        operand++;
    }
    return c;
}
