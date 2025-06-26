#include "exporter.h"

/*
 * Export a number in 5-digit octal format
 * @param num The number to be exported
 * @param line The line number in the output file
 * @param file The file pointer to write the output to
 */
void export_octal(int num, int line, FILE *file) {
    unsigned int unum = (unsigned int) num;
    char buffer[12];  /* Enough for 32-bit int in octal */
    int index = 11;
    int i;
    buffer[11] = '\0';
    /* Convert to octal */
    for (i = 0; i < 5; i++) {
        buffer[--index] = (char) ('0' + (unum & 7));
        unum >>= 3;
    }

    /* Print the result */
    fprintf(file, "%04d %.5s\n", line, &buffer[index]);
}


int export_files(struct translator *program, char *file_name) {
    int i;
    FILE *ob_file;
    FILE *ext_file;
    FILE *ent_file;
    char *ob_file_name, *ext_file_name, *ent_file_name;

    /* Allocate memory for file names */
    ob_file_name = malloc(strlen(file_name) + 4);
    ext_file_name = malloc(strlen(file_name) + 5);
    ent_file_name = malloc(strlen(file_name) + 5);
    if (ob_file_name == NULL || ext_file_name == NULL || ent_file_name == NULL) {
        fprintf(stderr, "Error: Cannot allocate memory\n");
        return -1;
    }

    /* Create file names */
    strcpy(ob_file_name, file_name);
    strcpy(ext_file_name, file_name);
    strcpy(ent_file_name, file_name);
    strcat(ob_file_name, ".ob");
    strcat(ext_file_name, ".ext");
    strcat(ent_file_name, ".ent");

    /* Write object file */
    ob_file = fopen(ob_file_name, "w");
    fprintf(ob_file, "  %d %d\n", program->IC, program->DC);

    /* Write code image */
    for (i = 0; i < program->IC; ++i) {
        if (program->code_image[i] < 0) {
            export_octal(program->code_image[i], i + 100, ob_file);
        } else {
            fprintf(ob_file, "%04d %05o\n", i + 100, program->code_image[i]);
        }
    }

    /* Write data image */
    for (i = 0; i < program->DC; ++i) {
        if (program->data_image[i] < 0) {
            export_octal(program->data_image[i], i + 100 + program->IC, ob_file);
        } else {
            fprintf(ob_file, "%04d %05o\n", i + 100 + program->IC, program->data_image[i]);
        }
    }

    /* Write externals file if there are external references */
    if (program->externals_count != 0) {
        ext_file = fopen(ext_file_name, "w");
        for (i = 0; i < program->externals_count; ++i) {
            fprintf(ext_file, "%s %04d\n", program->externals[i].name, (*(program->externals[i].address)) + 100);
        }
        fclose(ext_file);
    }

    /* Write entries file if there are entry points */
    if (program->entries_count != 0) {
        ent_file = fopen(ent_file_name, "w");
        for (i = 0; i < program->entries_count; ++i) {
            fprintf(ent_file, "%s %04d\n", program->entries[i].name, (*(program->entries[i].address)) + 100);
        }
        fclose(ent_file);
    }

    /* Clean up */
    free(ob_file_name);
    free(ext_file_name);
    free(ent_file_name);
    fclose(ob_file);

    return 0;
}