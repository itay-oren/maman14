#include "line_disasseble.h"

char *my_strtok_r(char *str, const char *delim, char **saveptr) {
    char *token;

    if (str == NULL) {
        str = *saveptr;
    }

    /* Skip leading delimiters */
    str += strspn(str, delim);
    if (*str == '\0') {
        return NULL;
    }

    /* Find the end of the token */
    token = str;
    str = strpbrk(token, delim);
    if (str == NULL) {
        *saveptr = strchr(token, '\0');
    } else {
        *str = '\0';
        *saveptr = str + 1;
    }

    return token;
}

int find_char_index(const char *str, char target) {
    int index = 0;

    /* Traverse the string until the end */
    while (str[index] != '\0') {
        if (str[index] == target) {
            return index;  /* Return the index if character is found */
        }
        index++;
    }

    return -1;  /* Return -1 if the character is not found */
}

struct line_disassembly disassemble_line(const char *input) {
    struct line_disassembly result = {0}; /* Initialize result structure to zero */
    char *token;
    char *input_copy;
    char *saveptr;
    char *start;
    char *end;
    char *pos;
    int i;
    int index;

    /* Allocate memory for a copy of the input string */
    input_copy = malloc(strlen(input) + 1);
    if (input_copy == NULL) {
        printf("Error: Cannot allocate memory\n");
        exit(1);
    }
    strcpy(input_copy, input);

    /* Remove newline character from the end of the string if present */
    if (input_copy[strlen(input_copy) - 1] == '\n') {
        input_copy[strlen(input_copy) - 1] = '\0';
    }

    /* Find the index of the first '.' character */
    index = find_char_index(input_copy, '.');

    /* Check if the line starts with ".string" */
    if (strncmp(input_copy + ((index == -1) ? 0 : index), ".string", 7) == 0) {
        i = 0;

        /* Check for a label (ends with ':') */
        index = find_char_index(input_copy, ':');
        if (index != -1) {
            /* Allocate memory for and store the label */
            result.strings[i] = malloc(index + 2);
            if (result.strings[i] == NULL) {
                printf("Error: Cannot allocate memory\n");
                exit(1);
            }
            strncpy(result.strings[i], input_copy, index + 1);
            result.strings[i++][index + 1] = '\0';
            result.strings_count = 1;
        }

        /* Store ".string" directive */
        result.strings[i] = malloc(strlen(".string") + 1);
        if (result.strings[i] == NULL) {
            printf("Error: Cannot allocate memory\n");
            exit(1);
        }
        strcpy(result.strings[i++], ".string");
        result.strings_count++;

        /* Extract the string content (between quotes) */
        start = strchr(input_copy, '"');
        end = strrchr(input_copy, '"');
        if (start != NULL && end != NULL) {
            result.strings[i] = malloc(end - start + 2);
            if (result.strings[i] == NULL) {
                printf("Error: Cannot allocate memory\n");
                exit(1);
            }
            strncpy(result.strings[i], start, end - start + 1);
            result.strings[i++][end - start + 1] = '\0';
            result.strings_count++;
        }

        free(input_copy);
        return result;
    }

    /* If not a .string directive, tokenize the input */
    token = my_strtok_r(input_copy, " ", &saveptr);
    while (token != NULL && result.strings_count < MAX_LINE_LEN) {
        char *comma_pos = strchr(token, ',');

        if (comma_pos != NULL) {
            /* Handle case with comma */
            if (comma_pos != token) {
                /* Store characters before comma */
                *comma_pos = '\0';
                result.strings[result.strings_count] = malloc(comma_pos - token + 1);
                if (result.strings[result.strings_count] == NULL) {
                    printf("Error: Cannot allocate memory\n");
                    exit(1);
                }
                strcpy(result.strings[result.strings_count], token);
                result.strings_count++;
            }

            /* Store comma as separate string */
            result.strings[result.strings_count] = malloc(2);
            if (result.strings[result.strings_count] == NULL) {
                printf("Error: Cannot allocate memory\n");
                exit(1);
            }
            strcpy(result.strings[result.strings_count], ",");
            result.strings_count++;

            /* Store characters after comma (if any) */
            if (*(comma_pos + 1) != '\0') {
                result.strings[result.strings_count] = malloc(strlen(comma_pos + 1) + 1);
                if (result.strings[result.strings_count] == NULL) {
                    printf("Error: Cannot allocate memory\n");
                    exit(1);
                }
                strcpy(result.strings[result.strings_count], comma_pos + 1);
                result.strings_count++;
            }
        } else {
            /* No comma, store whole token */
            result.strings[result.strings_count] = malloc(strlen(token) + 1);
            if (result.strings[result.strings_count] == NULL) {
                printf("Error: Cannot allocate memory\n");
                exit(1);
            }
            strcpy(result.strings[result.strings_count], token);
            result.strings_count++;
        }

        token = my_strtok_r(NULL, " ", &saveptr);
    }

    /* Remove special characters (e.g., '\r') from each substring */
    for (i = 0; i < result.strings_count; i++) {
        pos = strchr(result.strings[i], '\r');
        if (pos != NULL) {
            *pos = '\0';
        }
    }

    free(input_copy);
    return result;
}

void free_line_disassembly(struct line_disassembly *ld) {
    int i;

    if (ld == NULL) {
        return;
    }

    for (i = 0; i < ld->strings_count; i++) {
        if (ld->strings[i] != NULL) {
            free(ld->strings[i]);
            ld->strings[i] = NULL;
        }
    }

    ld->strings_count = 0;
}
