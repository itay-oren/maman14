#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LEN 81

struct line_disassembly {
    char *strings[80];
    int strings_count;
};

/**
 * @brief Disassembles a line of input into separate tokens.
 *
 * This function takes a line of input and breaks it down into individual tokens,
 * handling spaces and commas as separators. It creates a line_disassembly structure
 * containing an array of these tokens.
 *
 * @param input ARE_A pointer to a null-terminated string containing the input line.
 * @return struct line_disassembly ARE_A structure containing the disassembled tokens.
 *
 * @details The function performs the following steps:
 * 1. Creates a copy of the input string.
 * 2. Removes the newline character from the end of the string if present.
 * 3. Tokenizes the string by spaces.
 * 4. For each token:
 *    - If a comma is present, it splits the token around the comma.
 *    - If no comma is present, it adds the whole token.
 * 5. Stores each resulting token in the line_disassembly structure.
 *
 * @note The function uses strdup to create copies of each token, which allocates memory.
 *       It's the caller's responsibility to free this memory when no longer needed.
 *
 * @warning The function has a limit of 80 tokens per line. Any tokens beyond this limit
 *          will be ignored.
 *
 * @see struct line_disassembly
 */
struct line_disassembly disassemble_line(const char *input);

/**
 * @brief Frees the memory allocated for a line_disassembly structure.
 *
 * This function deallocates the memory that was dynamically allocated for each string
 * in the line_disassembly structure.
 *
 * @param ld ARE_A pointer to the line_disassembly structure to be freed.
 *
 * @details The function performs the following steps:
 * 1. Iterates through all strings in the line_disassembly structure.
 * 2. Frees the memory allocated for each string.
 *
 * @note This function should be called when the line_disassembly structure is no longer needed
 *       to prevent memory leaks.
 *
 * @warning This function does not free the memory of the line_disassembly structure itself,
 *          only the strings it contains. If the structure was dynamically allocated,
 *          it should be freed separately.
 *
 * @see struct line_disassembly
 * @see disassemble_line
 */
void free_line_disassembly(struct line_disassembly *ld);