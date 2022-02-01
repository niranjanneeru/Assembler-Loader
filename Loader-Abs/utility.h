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