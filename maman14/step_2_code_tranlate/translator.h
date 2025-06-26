#include "../helpers/template.h"
#include "../data_structures/tabel.h"

#define LINE_SIZE 81

/**
 * @brief Performs the first pass of the assembly process.
 *
 * This function reads the input file line by line, processes labels, directives, and instructions,
 * and builds the initial symbol table. It also calculates the Instruction Counter (IC) and Data Counter (DC).
 *
 * @param program Pointer to the translator structure to store assembled program data.
 * @param file_name Name of the input file (used for error reporting).
 * @param input Pointer to the input file.
 * @param mcr_table Pointer to the macro table.
 * @return int Returns 0 if successful, 1 if errors were encountered.
 *
 * @details The function performs the following steps:
 * 1. Reads each line of the input file.
 * 2. Processes the line using get_line_template().
 * 3. Handles labels, updating the symbol table as necessary.
 * 4. Updates IC or DC based on the line type.
 * 5. Processes entry and extern directives.
 * 6. After processing all lines, performs a final pass to:
 *    - Check for undefined entry labels.
 *    - Update addresses of data labels.
 *
 * @note The function uses helper functions like get_line_template, search, add_label, and ic_dc_calc.
 *
 * @warning This function allocates memory for labels. Ensure proper memory management in the calling function.
 *
 * @see struct translator
 * @see struct template
 * @see struct label
 * @see get_line_template
 * @see add_label
 * @see ic_dc_calc
 */
int first_translate(struct translator *program, char *file_name, FILE *input,macro_table *mcr_table);

/**
 * @brief Performs the second pass of the assembly process.
 *
 * This function reads the input file again, generates machine code for instructions,
 * processes data directives, and resolves symbol references.
 *
 * @param program Pointer to the translator structure containing assembled program data.
 * @param file_name Name of the input file (used for error reporting).
 * @param input Pointer to the input file.
 * @return int Returns 0 if successful, 1 if errors were encountered.
 *
 * @details The function performs the following steps:
 * 1. Reads each line of the input file.
 * 2. Processes the line using get_line_template().
 * 3. For instructions:
 *    - Generates the first word of the instruction (opcode, addressing methods).
 *    - Generates additional words for operands.
 *    - Handles different addressing methods (immediate, direct, register).
 * 4. For data directives:
 *    - Processes .data and .string directives.
 *    - Adds data to the data image.
 * 5. Resolves symbol references and handles external symbols.
 * 6. Updates the code and data images in the program structure.
 *
 * @note The function uses helper functions like get_line_template, search, add_code_image,
 *       add_data_image, if_external, and is_external.
 *
 * @warning This function assumes that the first pass (first_translate) has been completed
 *          successfully and that the symbol table is properly populated.
 *
 * @see struct translator
 * @see struct template
 * @see struct label
 * @see get_line_template
 * @see add_code_image
 * @see add_data_image
 * @see if_external
 * @see is_external
 */
int second_translate(struct translator *program, char *file_name, FILE *input, macro_table *mcr_table);

/**
 * Calculate the increase in IC (Instruction Counter) or DC (Data Counter)
 * based on the current line
 * @param line The template of the current line
 * @param current The current IC or DC value
 * @return The new IC or DC value after processing the line
 */
int ic_dc_calc(struct template line, int current);