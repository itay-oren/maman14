#include "makro_handller.h"

int macro_handler(FILE *as_file, char *file_name,macro_table *mcr_table) {
    char line[LINE_BUFFER];
    char *output_file_name = malloc(strlen(file_name) + 4);
    struct line_disassembly new_line;
    int line_count = 0;
    int error = 0;
    int buffer_line_count = 0;
    int macro_flag = 0;

    char *macro_name = NULL;
    macro *macro_ptr = NULL;


    /* Buffer to store lines temporarily */
    char *buffer = NULL;
    int buffer_size = 0;
    int buffer_capacity = MAX_LINES_CUONT * LINE_BUFFER; /* Adjust buffer capacity to handle 3995 lines */
    if (output_file_name == NULL) {
        printf("Error: Cannot allocate memory\n");
        exit(1);
    }
    buffer = malloc(buffer_capacity);
    if (buffer == NULL) {
        printf("Error: Cannot allocate memory\n");
        exit(1);
    }

    strcpy(output_file_name, file_name);
    strcat(output_file_name, ".am");
    strcat(file_name, ".as");

    while (fgets(line, LINE_BUFFER, as_file) != NULL) {
        line_count++;
        new_line = disassemble_line(line);
        if (new_line.strings_count == 0) {
            continue;
        }
        if (strlen(line) > MAX_LINE_LEN) {
            printf("in %s: line: %d Error: Line exceeding maximum length\n", file_name, line_count--);
            error = -1;
        }


        switch (check_line_type(new_line, mcr_table, macro_flag)) {
            case MACRO_DEFINITION:
                error = 0;
                if (new_line.strings_count < 2) {
                    printf("in %s: line: %d Error: macro without name definition\n", file_name, line_count);
                    error = -1;
                }
                if (new_line.strings_count > 2) {
                    printf("in %s: line: %d Error: extra text after macro name\n", file_name, line_count);
                    error = -1;
                }
                macro_name = new_line.strings[1];
                if (is_reserved_word(macro_name)) {
                    printf("in %s: line: %d Error: Invalid macro name - %s is reserved word\n", file_name, line_count,
                           macro_name);
                    error = -1;
                }

                if(macro_flag){
                    if (get_macro(mcr_table, macro_name)) {
                        printf("in %s: line: %d Error: macro name already exists\n", file_name, line_count);
                        error = -1;
                    }
                }


                macro_ptr = add_macro(macro_name);
                break;
            case END_MACRO:
                if (new_line.strings_count > 1) {
                    printf("in %s: line: %d Error: extra text after end of macro\n", file_name, line_count);
                    error = -1;
                }
                if (error == -1) {
                    break;
                }
                if (macro_flag == 0) {
                    mcr_table->macro->macro_name =  malloc(strlen(macro_ptr->macro_name)+1);
                    if (mcr_table->macro->macro_name == NULL) {
                        printf("Error: Cannot allocate memory\n");
                        exit(1);
                    }
                    strncpy(mcr_table->macro->macro_name, macro_ptr->macro_name, strlen(macro_ptr->macro_name));
                    mcr_table->macro->lines = (char *) malloc(strlen(macro_ptr->lines) + 1);
                    if (mcr_table->macro->lines == NULL) {
                        printf("Error: Cannot allocate memory\n");
                        exit(1);
                    }
                    strcpy(mcr_table->macro->lines, macro_ptr->lines);
                    mcr_table->macro->lines_count = macro_ptr->lines_count;
                    mcr_table->next = NULL;

                } else {
                    add_macro_table(mcr_table, macro_ptr);
                }
                macro_flag = 1;
                if (macro_ptr != NULL) {
                    free(macro_ptr->macro_name);
                    free(macro_ptr->lines);
                    free(macro_ptr);
                }
                macro_ptr = NULL;
                break;
            case MACRO_CALL:
                if (new_line.strings_count > 1) {
                    printf("in %s: line: %d Error: extra text after macro name\n", file_name, line_count);
                    error = -1;
                }
                macro_ptr = get_macro(mcr_table, new_line.strings[0]);
                /* Append macro lines to buffer */
                if (buffer_size + strlen(macro_ptr->lines) >= buffer_capacity || buffer_line_count >= 3995) {
                    error = -1;
                    break;
                }
                strcpy(buffer + buffer_size, macro_ptr->lines);
                buffer_size += strlen(macro_ptr->lines);
                buffer_line_count += macro_ptr->lines_count;
                macro_ptr = NULL;
                break;
            case REGULAR_LINE:
                if (macro_ptr != NULL) {
                    add_new_line(macro_ptr, line);
                } else {
                    /* Append line to buffer */
                    if (buffer_size + strlen(line) >= buffer_capacity || buffer_line_count >= 3995) {
                        error = -1;
                        break;
                    }
                    strcpy(buffer + buffer_size, line);
                    buffer_size += strlen(line);
                    buffer_line_count++;
                }
                break;
            case ERROR:
                printf("in %s: line: %d Error: Invalid line\n", file_name, line_count);
                error = -1;
        }
        free_line_disassembly(&new_line);
    }

    if (error == 0) {
        FILE *am_file = fopen(output_file_name, "w");
        if (am_file != NULL) {
            fputs(buffer, am_file);
            fclose(am_file);
        } else {
            printf("Error: Could not open output file %s\n", output_file_name);
            error = -1;
        }
    }

    free(output_file_name);
    free(buffer);

    return error == 0 ? 0 : -1;
}


int check_line_type(struct line_disassembly line, macro_table *mcr_table, int macro_flag) {
    /*check if the line starts with the word "macr" after ignore spaces
     * else check if the line starts with "endmacr"
     * else check if the line starts with a macro name
     * */
    struct macro *macro_ptr;

    if (strcmp(line.strings[0], "macr") == 0) {
        return MACRO_DEFINITION;
    }
    if (strcmp(line.strings[0], "endmacr") == 0) {
        return END_MACRO;
    }
    if (macro_flag) {
        macro_ptr = get_macro(mcr_table, line.strings[0]);
        if (macro_ptr) {
            return MACRO_CALL;
        }
    }
    return REGULAR_LINE;
}