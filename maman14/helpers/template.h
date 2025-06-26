#include "../step_1_preprocessor/makro_handller.h"

#define TRUE 1
#define FALSE 0

/*
 * Structure to represent a line template in the driver
 */
struct template {
    char *syntax_error; /* store the error */

    struct { /* if label */
        char *name;
        int address;
    } label;

    /* Define the type of the line */
    enum {
        empty,
        directive,
        instruction
    } type_of_line;

    /* Union to store different types of line content */
    union {
        /* For directive lines */
        struct {
            enum {
                entry,
                data,
                string,
                exter
            } dir_type;
            union {
                int data[80];
                char *string;
                char *label_entr_extr;
            } dir_data;
        } is_dir;

        /* For instruction lines */
        struct {
            enum {
                mov, cmp, add, sub,
                lea, clr, not, inc,
                dec, jmp, bne, red,
                prn, jsr, rts, stop
            } inst_type;
            struct {
                enum {
                    immediate = 7,
                    direct = 8,
                    indirect_register = 9,
                    direct_register = 10,
                    no_operand = 0
                } addr_method;
                union {
                    int immediate;
                    char *label;
                    unsigned int register_address;
                } op_addr_method;
            } op[2];
        } is_inst;
    } option;
};

/* Function prototypes */

/**
 * Initialize a new template structure
 * @return An initialized template structure
 */
struct template init_template();

/**
 * @brief Processes a line of assembly code and creates a template structure.
 *
 * This function takes a line of assembly code, disassembles it, and creates a
 * template structure containing information about the line's type, label,
 * directive, or instruction details.
 *
 * @param line ARE_A pointer to a null-terminated string containing the line of assembly code.
 * @return struct template ARE_A structure containing the processed information about the line.
 *
 * @details The function performs the following steps:
 * 1. Disassembles the input line.
 * 2. Initializes a template structure.
 * 3. Handles empty lines and comments.
 * 4. Checks for and processes label declarations.
 * 5. Determines the line type (directive or instruction) and processes accordingly.
 * 6. For directives, it handles .data, .string, .entry, and .extern types.
 * 7. For instructions, it processes the appropriate number of operands based on the instruction type.
 * 8. Sets syntax error messages if any issues are encountered during processing.
 *
 * @note The function uses several helper functions like disassemble_line, is_lable_dec,
 *       line_type, directive_type, instruction_type, and handel_addressing.
 *
 * @warning This function allocates memory for certain fields in the template structure.
 *          It's the caller's responsibility to free this memory when no longer needed.
 */
struct template get_line_template(char *line, macro_table *mcr_table);

/**
 * Check if a string is a valid label declaration
 * @param str The string to check
 * @return 1 if valid label declaration, 0 otherwise
 */
int is_lable_dec(char *str,struct template *temp, macro_table *mcr_table);

/**
 * Check if a string is a valid label
 * @param str The string to check
 * @return 1 if valid label, 0 otherwise
 */
int is_lable(char *str);

/**
 * Determine the type of a line
 * @param str The line to check
 * @return The line type (empty, directive, or instruction)
 */
int line_type(char *str);

/**
 * Determine the type of a directive
 * @param str The directive to check
 * @return The directive type (entry, data, string, or exter)
 */
int directive_type(char *str);

/**
 * Determine the type of an instruction and update the template
 * @param str The instruction to check
 * @param temp Pointer to the template structure to update
 * @return The instruction type
 */
int instruction_type(char *str, struct template *temp);

/**
 * Handles the addressing mode of an operand in an assembly instruction.
 *
 * This function determines the addressing mode of the given operand string
 * and updates the corresponding fields in the template structure.
 *
 * @param str The operand string to be analyzed.
 * @param temp Pointer to the template structure to be updated.
 * @param index The index of the operand in the instruction.
 */
int handel_addressing(char *str, struct template *temp, int index);

/**
 * Check if a string represents a valid number
 * @param str The string to check
 * @return 1 if valid number, 0 otherwise
 */
int is_number(const char *str);

void free_template(struct template *temp);