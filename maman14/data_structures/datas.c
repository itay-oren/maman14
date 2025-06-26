#include "datas.h"


int is_instruction(char *word) {
    const char *instruction_names[] = {
            "mov", "cmp", "add", "sub",
            "lea", "clr", "not", "inc",
            "dec", "jmp", "bne", "red",
            "prn", "jsr", "rts", "stop"
    };
    int i;
    for (i = 0; i < 16; ++i) {
        if (strcmp(word, instruction_names[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int is_reserved_word(char *word) {
    const char *instruction_names[] = {
            "mov", "cmp", "add", "sub",
            "lea", "clr", "not", "inc",
            "dec", "jmp", "bne", "red",
            "prn", "jsr", "rts", "stop"
    };
    const char *directiv[] = {
            ".data", ".string", ".entry", ".extern", ".endmacr", ".macr"
    };
    const char *reg[] = {
            "r1", "r2", "r3", "r4", "r5", "r6", "r7"
    };
    int i;
    for (i = 0; i < 16; ++i) {
        if (strcmp(word, instruction_names[i]) == 0) {
            return 1;
        }
    }
    for (i = 0; i < 6; ++i) {
        if (strcmp(word, directiv[i]) == 0) {
            return 1;
        }
    }
    for (i = 0; i < 7; i++) {
        if (strcmp(word, reg[i]) == 0) {
            return 1;
        }
    }
    return 0;
}
