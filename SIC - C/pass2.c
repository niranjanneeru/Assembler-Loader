# include "pass1.c"

FILE *listing_fp;
FILE *obj_code;
int txt_record_count = 0;
char txt_record[70];
bool is_active = false;


typedef struct {
    char *addr;
    char *opcode;
    char *operand;
} entry_intermediate_file;

void convert_to_object_code(entry_intermediate_file line);


void assemble(entry_intermediate_file line, long addr, int loc);

void pad(char code[7], char hex[5]);

void manage_text_record(const char *string, int i, char *addr);

void create_text_record(char *);

void pass2() {
    intermediate_fp = fopen("intermediate.out", "r");
    if (intermediate_fp == NULL) {
        perror("Unable to Open Intermediate File\n");
        exit(EXIT_FAILURE);
    }
    listing_fp = fopen("listing.out", "w");
    obj_code = fopen("obj_code.out", "w");

    char intermediate_line[100];
    while (fgets(intermediate_line, sizeof(intermediate_line), intermediate_fp)) {
        if (intermediate_line[strlen(intermediate_line) - 1] == '\n') {
            intermediate_line[strlen(intermediate_line) - 1] = '\0';
        }
        entry_intermediate_file line;
        line.addr = strtok(intermediate_line, " ");
        line.opcode = strtok(NULL, " ");
        line.operand = strtok(NULL, " ");
//        printf("%s %s %s\n", line.addr, line.opcode, line.operand); // debug
        convert_to_object_code(line);
    }
    fclose(intermediate_fp);
    fclose(listing_fp);
    fclose(obj_code);
}

void write_to_listing_file(entry_intermediate_file line) {
    fprintf(listing_fp, "%s %s %s\n", line.addr, line.opcode, line.operand);
}

void write_header_record() {
    char addr1[5];
    sprintf(addr1, "%lX", starting_addr);
    char addr3[5];
    for (int i = 0; i < 4; ++i) {
        if (4 - strlen(addr1) > i) {
            addr3[i] = '0';
        } else {
            addr3[i] = addr1[i - (4 - strlen(addr1))];
        }
    }
    addr3[4] = '\0';
    char addr2[5];
    sprintf(addr2, "%lX", program_size);
    char addr4[5];
    for (int i = 0; i < 4; ++i) {
        if (4 - strlen(addr2) > i) {
            addr4[i] = '0';
        } else {
            addr4[i] = addr2[i - (4 - strlen(addr2))];
        }
    }
    addr4[4] = '\0';
    if (program_name) {
        char name[7];
        for (int i = 0; i < 6; ++i) {
            if (strlen(SYM_TAB.symTab[0].symbol) > i) {
                name[i] = SYM_TAB.symTab[0].symbol[i];
            } else {
                name[i] = ' ';
            }
        }
        fprintf(obj_code, "H%s00%s00%s\n", name, addr3, addr4);
    } else {
        fprintf(obj_code, "H      00%s00%s\n", addr3, addr4);
    }
}


void convert_to_object_code(entry_intermediate_file line) {
    if (strcmp(line.opcode, "START") == 0) {
        write_to_listing_file(line);
        write_header_record();
        char s_addr[7];
        s_addr[0] = '0';
        s_addr[1] = '0';
        for (int i = 0; i < 4; ++i) {
            if (4 - strlen(line.addr) > i) {
                s_addr[i + 2] = '0';
            } else {
                s_addr[i + 2] = line.addr[i];
            }
        }
        return;
    }

    // OP CODE
    long int operand_addr;
    int loc = search_op_tab(line.opcode);
    if (loc != -1) {
        write_to_listing_file(line);
        if (strcmp(line.opcode, "RSUB") == 0) {
            operand_addr = 0;
        } else {
            int flag = 0;
            if (line.operand[strlen(line.operand) - 1] == 'X' && line.operand[strlen(line.operand) - 2] == ',') {
                flag = 1;
                line.operand[strlen(line.operand) - 2] = '\0';
            }
            int iloc = search_sym_tab(line.operand);
            if (iloc == -1) {
                fprintf(listing_fp, "Symbol Not Found\n");
                perror("Symbol Not Found\n");
                printf("%s\n", line.operand);
                operand_addr = 0;
            } else {
                operand_addr = SYM_TAB.symTab[iloc].addr;
            }
            if (flag) {
                line.operand[strlen(line.operand)] = ',';
            }
        }
        assemble(line, operand_addr, loc);
        return;
    }

    // ASSEMBLY DIRECTIVES
    if (strcmp(line.opcode, "RESW") == 0 || strcmp(line.opcode, "RESB") == 0) {
        write_to_listing_file(line);
        if (is_active) {
            int len = ceil((txt_record_count - 9) / 2.0);
            char hex[3];
            sprintf(hex, "%X", len);
            if (strlen(hex) == 1) {
                hex[1] = hex[0];
                hex[0] = '0';
            }
            txt_record[7] = hex[0];
            txt_record[8] = hex[1];
            txt_record[txt_record_count] = '\0';
            fprintf(obj_code, "%s\n", txt_record);
            is_active = false;
        }
        return;
    }

    if (strcmp(line.opcode, "END") == 0) {
        write_to_listing_file(line);
        if (is_active) {
            int len = ceil((txt_record_count - 9) / 2.0);
            char hex[3];
            sprintf(hex, "%X", len);
            if (strlen(hex) == 1) {
                hex[1] = hex[0];
                hex[0] = '0';
            }
            txt_record[7] = hex[0];
            txt_record[8] = hex[1];
            txt_record[txt_record_count] = '\0';
            fprintf(obj_code, "%s\n", txt_record);
            is_active = false;
        }
        if (line.operand) {
            char e_addr[7];
            e_addr[0] = '0';
            e_addr[1] = '0';
            pad(e_addr, line.operand);
            fprintf(obj_code, "E%s\n", e_addr);
//            printf("%s", e_addr);
        } else {
            fprintf(obj_code, "E00%lX", starting_addr);
        }
        return;
    }

    if (strcmp(line.opcode, "WORD") == 0) {
        write_to_listing_file(line);
        char hex[7];
        sprintf(hex, "%X", atoi(line.operand));
        char padded[7];
        int thresh = 6 - strlen(hex);
        for (int i = 0; i < 6; ++i) {
            if (thresh > i) {
                padded[i] = '0';
            } else {
                padded[i] = hex[i - thresh];
            }
        }
        char loc_addr[7];
        loc_addr[0] = '0';
        loc_addr[1] = '0';
        pad(loc_addr, line.addr);
        manage_text_record(padded, 6, loc_addr);
        return;
    }

    if (strcmp(line.opcode, "BYTE") == 0) {
        char loc_addr[7];
        loc_addr[0] = '0';
        loc_addr[1] = '0';
        pad(loc_addr, line.addr);
        if (line.operand[0] == 'X') {
            int len = strlen(line.operand) - 3;
            if (len % 2 == 0) {
                line.operand[strlen(line.operand) - 1] = '\0';
                manage_text_record(line.operand + 2, len, loc_addr);
            } else {
                line.operand[1] = '0';
                line.operand[strlen(line.operand) - 1] = '\0';
                manage_text_record(line.operand + 1, len + 1, loc_addr);
            }
        } else {
            for (int i = 2; i < strlen(line.operand) - 1; ++i) {
                char hex[3];
                sprintf(hex, "%X", (int) line.operand[i]);
                manage_text_record(hex, 2, loc_addr);
            }
        }
    }
}


void assemble(entry_intermediate_file line, long addr, int loc) {
    char *opcode = OP_TAB[loc].hex;
    if (strcmp(opcode, "4C") == 0) {
        char loc_addr[7];
        loc_addr[0] = '0';
        loc_addr[1] = '0';
        pad(loc_addr, line.addr);
        manage_text_record("4C0000", 6, loc_addr);
    } else {
        if (line.operand[strlen(line.operand) - 1] == 'X' && line.operand[strlen(line.operand) - 2] == ',') {
            addr += 8 * 16 * 16 * 16;
        }
        char hex[5];
        sprintf(hex, "%lX", addr);
        char object_code[7];
        object_code[0] = opcode[0];
        object_code[1] = opcode[1];
        pad(object_code, hex);
        char loc_addr[7];
        loc_addr[0] = '0';
        loc_addr[1] = '0';
        pad(loc_addr, line.addr);
        manage_text_record(object_code, 6, loc_addr);
    }
}

void manage_text_record(const char *string, int i, char *addr) {
//    printf("%s\n", string); //debug
    if (!is_active) {
        create_text_record(addr);
        manage_text_record(string, i, addr);
    } else if (txt_record_count <= 69 - i) {
        strncpy(txt_record + txt_record_count, string, i);
        txt_record_count += i;
    } else {
        int len = ceil((txt_record_count - 9) / 2.0);
        char hex[3];
        sprintf(hex, "%X", len);
        if (strlen(hex) == 1) {
            hex[1] = hex[0];
            hex[0] = '0';
        }
        txt_record[7] = hex[0];
        txt_record[8] = hex[1];
        txt_record[txt_record_count] = '\0';
        fprintf(obj_code, "%s\n", txt_record);
        is_active = false;
        create_text_record(addr);
        manage_text_record(string, i, addr);
    }
}

void pad(char code[7], char hex[5]) {
    for (int i = 0; i < 4; i++) {
        if (4 - strlen(hex) > i) {
            code[i + 2] = '0';
        } else {
            code[i + 2] = hex[i];
        }
    }
    code[6] = '\0';
}

void create_text_record(char *addr) {
    txt_record_count = 9;
    txt_record[0] = 'T';
    strncpy(txt_record + 1, addr, 6);
    is_active = true;
}