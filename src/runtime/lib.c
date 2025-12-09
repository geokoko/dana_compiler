#include <stdio.h>
#include <stdint.h>
#include <string.h>

void dana_writeInteger(int n) {
    printf("%d", n);
}

void dana_writeByte(int8_t c) {
    printf("%d", (int)c);
}

void dana_writeChar(char c) {
    printf("%c", c);
}

void dana_writeString(const char* s) {
    printf("%s", s);
}

int dana_readInteger(void) {
    int n;
    scanf("%d", &n);
    return n;
}

int8_t dana_readByte(void) {
    int tmp;
    scanf("%d", &tmp);
    return (int8_t)tmp;
}

char dana_readChar(void) {
    char c;
    scanf(" %c", &c);
    return c;
}

void dana_readString(int n, char* s) {
    if (n <= 0) return;

    if (!fgets(s, n, stdin)) {
        s[0] = '\0';
        return;
    }

    int i = 0;
    while (s[i] != '\0') {
        if (s[i] == '\n') {
            s[i] = '\0';
            break;
        }
        i++;
    }
}

int dana_extend(int8_t b) {
    return (int)b;
}

int8_t dana_shrink(int i) {
    return (int8_t)i;
}

int dana_strlen(const char* s) {
    return (int)strlen(s);  // Dana int
}

int dana_strcmp(const char* s1, const char* s2) {
    return strcmp(s1, s2);
}

void dana_strcpy(char* trg, const char* src) {
    strcpy(trg, src);
}

void dana_strcat(char* trg, const char* src) {
    strcat(trg, src);
}

