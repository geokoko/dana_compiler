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
    int n = 0;
    if (scanf("%d", &n) != 1) {
        // Clear input buffer on error
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
    return n;
}

int8_t dana_readByte(void) {
    int tmp = 0;
    if (scanf("%d", &tmp) != 1) {
        // Clear input buffer on error
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
    return (int8_t)tmp;
}

char dana_readChar(void) {
    char c = '\0';
    if (scanf(" %c", &c) != 1) {
        // Clear input buffer on error
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
    }
    return c;
}

void dana_readString(int n, char* s) {
    if (n <= 0 || !s) return;

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
    if (!s) return 0;
    return (int)strlen(s);  // Dana int
}

int dana_strcmp(const char* s1, const char* s2) {
    if (!s1 && !s2) return 0;
    if (!s1) return -1;
    if (!s2) return 1;
    return strcmp(s1, s2);
}

void dana_strcpy(char* trg, const char* src) {
    if (!trg || !src) return;
    strcpy(trg, src);
}

void dana_strcat(char* trg, const char* src) {
    if (!trg || !src) return;
    strcat(trg, src);
}

