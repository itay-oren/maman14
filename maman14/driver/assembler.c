#include "../step_3_assemble/exporter.h"

int main(int argc, char *argv[]) {
    int error;
    FILE *as_file = NULL;
    FILE *am_file = NULL;
    char *file_name = malloc(1); /* Initially allocate memory (size 1) */
    macro_table mcr_table;
    struct translator *program;
    if (file_name == NULL) {
        printf("Error: Cannot allocate memory\n");
        exit(1);
    }
    if (argc < 2) {
        printf("Error: No input files\n");
        return -1;
    }

    while (--argc > 0) {
        mcr_table.macro = init_macro();
        mcr_table.next = NULL;
        program = init_translator();
        free(file_name);
        file_name = malloc(strlen(argv[argc]) + 4);
        if (file_name == NULL) {
            printf("Error: Cannot allocate memory\n");
            free_translator(program);
            exit(1);
        }

        strcpy(file_name, argv[argc]);
        strcat(file_name, ".as");

        as_file = fopen(file_name, "r");
        if (as_file == NULL) {
            printf("Error: Cannot open as_file %s\n", file_name);
            free_translator(program);
            continue;  /* Skip to next iteration, don't return immediately */
        }

        strcpy(file_name, argv[argc]);

        error = macro_handler(as_file, file_name, &mcr_table);

        fclose(as_file);  /* Close after macro_handler, only once */
        as_file = NULL;   /* Set to NULL after closing */
        if (error == -1) {
            free_translator(program);
            continue;
        }

        strcpy(file_name, argv[argc]);
        strcat(file_name, ".am");

        am_file = fopen(file_name, "r");
        if (am_file == NULL) {
            printf("Error: Cannot open am_file %s\n", file_name);
            free_translator(program);
            continue;
        }

        error = first_translate(program, file_name, am_file, &mcr_table);
        if (error) {
            fclose(am_file);
            am_file = NULL;
            free_translator(program);
            continue;
        }
        fseek(am_file, 0, SEEK_SET);

        error = second_translate(program, file_name, am_file,&mcr_table);
        fclose(am_file);  /* Close after second_translate, only once */
        am_file = NULL;   /* Set to NULL after closing */
        if (error) {
            free_translator(program);
            continue;
        }

        error = export_files(program, argv[argc]);
        if (error) {
            free_translator(program);
            continue;
        }
        free_translator(program);
    }

    free(file_name); /* Free file_name after the loop */
    free_macro_table(&mcr_table);
    return 0;
}
