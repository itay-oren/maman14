#include "macro_data_structures.h"
#include "../helpers/line_disasseble.h"

#define MAX_LINES_CUONT 3995
enum line_type {
    MACRO_DEFINITION,
    END_MACRO,
    MACRO_CALL,
    REGULAR_LINE,
    ERROR
};

/**
 * @brief Handles macro expansion in an assembly source file.
 *
 * This function processes an assembly source file, expanding macros and creating
 * a new file with the expanded code.
 *
 * @param as_file Pointer to the input assembly file.
 * @param file_name Name of the input file (used to generate the output file name).
 * @return int Returns 0 on success, -1 on failure.
 *
 * @details The function performs the following steps:
 * 1. Creates an output file with the ".am" extension.
 * 2. Reads the input file line by line, processing each line based on its type:
 *    - MACRO_DEFINITION: Starts a new macro definition.
 *    - END_MACRO: Ends the current macro definition and adds it to the macro table.
 *    - MACRO_CALL: Expands the called macro by writing its contents to the output file.
 *    - REGULAR_LINE: Either adds the line to the current macro definition or writes it to the output file.
 * 3. Closes both input and output files.
 * 4. Frees allocated memory.
 *
 * @note The function uses several helper functions like check_line_type, get_macro_name,
 *       add_macro, add_macro_table, get_macro, and add_new_line.
 *
 * @warning This function allocates memory for the output file name and macro structures.
 *          It frees this memory before returning, but ensure proper error handling in case of early returns.
 *
 * @see macro
 * @see macro_table
 * @see add_macro
 * @see add_macro_table
 * @see get_macro
 * @see free_macro_table
 */
int macro_handler(FILE *as_file, char *file_name,macro_table *mcr_table);

int check_line_type(struct line_disassembly line, macro_table *mcr_table, int macro_flag);