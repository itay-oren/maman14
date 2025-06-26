#include "../data_structures/datas.h"


/**
 * @file
 * @brief Macros handling and storage.
 */

#define LINE_BUFFER 85        /**< The size of the buffer for storing lines. */
#define MAX_LINE_LEN 81       /**< The maximum length of a line. */

/**
 * @struct macro
 * @brief Represents a macro with its name and associated lines of code.
 *
 * @var macro::macro_name
 * The name of the macro.
 *
 * @var macro::lines
 * The lines associated with the macro, stored as a single string.
 *
 * @var macro::lines_count
 * The number of lines associated with the macro.
 */
typedef struct macro {
    char *macro_name;
    char *lines;
    int lines_count;
} macro;

/**
 * @struct macro_table
 * @brief Represents a node in a linked list of macros.
 *
 * @var macro_table::macro
 * A pointer to a macro.
 *
 * @var macro_table::next
 * A pointer to the next node in the list.
 */
typedef struct macro_table {
    macro *macro;
    struct macro_table *next;
} macro_table;

/**
 * @brief Creates a new macro.
 *
 * Allocates memory for a new macro structure and initializes its fields.
 *
 * @param macro_name The name of the macro.
 * @return A pointer to the newly created macro.
 */
macro *add_macro(char *macro_name);

/**
 * @brief Searches for a macro in the macro table.
 *
 * Traverses the linked list of macros to find one with a matching name.
 *
 * @param mcr_table The linked list of macros.
 * @param macro_name The name of the macro to search for.
 * @return A pointer to the found macro, or NULL if not found.
 */
macro *get_macro(macro_table *mcr_table, char *macro_name);

/**
 * @brief Adds a new line to the macro.
 *
 * Appends a new line of text to the macro's lines, reallocating memory as needed.
 *
 * @param mcr The macro to which the line should be added.
 * @param line The line of text to add.
 */
void add_new_line(macro *mcr, char *line);

/**
 * @brief Adds a macro to the macro table.
 *
 * Creates a new node in the linked list and inserts the macro into it.
 *
 * @param mcr_table The existing macro table.
 * @param mcr The macro to add to the table.
 * @return A pointer to the new head of the macro table.
 */
void add_macro_table(macro_table *mcr_table, macro *mcr)    ;

/**
 * @brief Frees the memory used by the macro table.
 *
 * Recursively frees all nodes in the linked list and their associated macros.
 *
 * @param mcr_table The macro table to free.
 */
void free_macro_table(macro_table *mcr_table);

macro_table *init_macro_table();
macro *init_macro();