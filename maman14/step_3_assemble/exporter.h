#include "../step_2_code_tranlate/translator.h"

/**
 * @brief Exports the assembled program to output files.
 *
 * This function creates and writes the object file (.ob), externals file (.ext),
 * and entries file (.ent) based on the assembled program data.
 *
 * @param program Pointer to the translator structure containing the assembled program data.
 * @param file Pointer to the input file (not used in the function, consider removing).
 * @param file_name Base name for the output files.
 * @return int Returns 0 on success, -1 on failure.
 *
 * @details The function performs the following steps:
 * 1. Allocates memory for output file names.
 * 2. Creates the object file (.ob) containing:
 *    - The instruction count (IC) and data count (DC).
 *    - The code image in octal format.
 *    - The data image in octal format.
 * 3. Creates the externals file (.ext) if there are external references.
 * 4. Creates the entries file (.ent) if there are entry points.
 * 5. Frees allocated memory and closes files.
 *
 * @note The function uses helper functions like print_table and export_octal.
 *
 * @warning This function allocates memory for file names. It frees this memory before
 *          returning, but ensure proper error handling in case of early returns.
 *
 * @see struct translator
 * @see export_octal
 * @see print_table
 */
int export_files(struct translator *program, char *file_name);