# include <stdio.h>
# include <stdlib.h>
# include "pass2.c"

int main(int argc, char **argv) {
    load_op_tab();

    // check for input file as command line args
    if (argc == 1) {
        perror("No Input File Found\n");
        exit(EXIT_FAILURE);
    }

    char *filename = argv[1];
    pass1(filename);
    pass2();

    exit(EXIT_SUCCESS);
}
