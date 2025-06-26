#include "template.h"

/**
 * Removes all occurrences of a specified character from a string.
 *
 * This function modifies the original string in-place, removing
 * all instances of the specified 'garbage' character.
 *
 * @param str The input string to be modified. If NULL, the function returns NULL.
 * @param garbage The character to be removed from the string.
 * @return ARE_A pointer to the modified string, or NULL if the input was NULL.
 */
char *removeChar(char *str, char garbage) {
    char *src = str;
    char *dst = str;
    if (str == NULL) return NULL;  /* Check for null input */
    while (*src) {
        if (*src != garbage) {
            *dst = *src;
            dst++;
        }
        src++;
    }
    *dst = '\0';  /* Ensure null-termination */

    return str;
}

struct template get_line_template(char *line, macro_table *mcr_table) {
    int i = 0;
    int j;
    int test;
    int inst;
    int addr;
    struct line_disassembly new_line = disassemble_line(line);
    struct template temp = init_template();
    temp.syntax_error = NULL;

    /* Handle empty lines or comments */
    if (line[0] == '\n' || line[0] == ';') {
        temp.type_of_line = empty;
        free_line_disassembly(&new_line);
        return temp;
    }

    /* Check for label declaration */
    test = is_lable_dec(new_line.strings[i], &temp, mcr_table);
    if (test == 2) {
        temp.label.name = NULL;
    } else if (test == 1) {
        temp.label.name = new_line.strings[i];
        /* Remove the label from the new_line.strings */
        for (j = 0; j < new_line.strings_count; ++j) {
            new_line.strings[j] = new_line.strings[j + 1];
        }
        new_line.strings_count--;
        if (new_line.strings_count == 0) {
            temp.type_of_line = empty;
            free_line_disassembly(&new_line);
            return temp;
        }
    } else {
        free_line_disassembly(&new_line);
        return temp;
    }

    /* Process the line based on its type */
    switch (line_type(new_line.strings[i])) {
        case directive:
            temp.type_of_line = directive;
            switch (directive_type(new_line.strings[i++])) {
                case data:
                    /* Handle .data directive */
                    temp.option.is_dir.dir_type = data;
                    if(new_line.strings_count == 1){
                        temp.syntax_error = "Error: missing .data arguments";
                        free_line_disassembly(&new_line);
                        return temp;
                    }
                    if (new_line.strings_count % 2 != 0) {
                        temp.syntax_error = "Error: invalid data";
                        free_line_disassembly(&new_line);
                        return temp;
                    }
                    for (j = 1; i < new_line.strings_count; ++i) {
                        if (i % 2 != 0) {
                            if (strcmp(new_line.strings[i], ",") == 0) {
                                temp.syntax_error = "Error: invalid comma";
                                free_line_disassembly(&new_line);
                                return temp;
                            }
                            if (!is_number(new_line.strings[i])) {
                                temp.syntax_error = "Error: invalid data";
                                free_line_disassembly(&new_line);
                                return temp;
                            }
                            temp.option.is_dir.dir_data.data[j++] = strtol(new_line.strings[i], NULL, 10);
                            temp.option.is_dir.dir_data.data[0]++;
                        } else if (strcmp(new_line.strings[i], ",") != 0) {
                            temp.syntax_error = "Error: missing comma";
                            free_line_disassembly(&new_line);
                            return temp;
                        }
                    }
                    break;
                case string:
                    /* Handle .string directive */
                    if (new_line.strings_count == 1) {
                        temp.syntax_error = "Error: missing string";
                        free_line_disassembly(&new_line);
                        return temp;
                    }
                    if (new_line.strings[1][0] != '"') {
                        temp.syntax_error = "Error: unopened string";
                        free_line_disassembly(&new_line);
                        return temp;
                    }
                    if (new_line.strings[1][strlen(new_line.strings[1]) - 1] != '"') {
                        temp.syntax_error = "Error: unclosed string";
                        free_line_disassembly(&new_line);
                        return temp;
                    }
                    if (new_line.strings_count != 2) {
                        temp.syntax_error = "Error: invalid string";
                        free_line_disassembly(&new_line);
                        return temp;
                    }
                    temp.option.is_dir.dir_type = string;
                    temp.option.is_dir.dir_data.string = malloc(80 * sizeof(char));
                    strcpy(temp.option.is_dir.dir_data.string, removeChar((new_line.strings[i]), '"'));
                    break;
                case entry:
                    /* Handle .entry directive */
                    if(new_line.strings_count>2){
                        temp.syntax_error = "Error: Extraneous text in command or after end of command";
                        free_line_disassembly(&new_line);
                        return temp;
                    }
                    temp.option.is_dir.dir_type = entry;
                    temp.option.is_dir.dir_data.label_entr_extr = malloc(80 * sizeof(char));
                    strcpy(temp.option.is_dir.dir_data.label_entr_extr, new_line.strings[i]);
                    break;
                case exter:
                    if(new_line.strings_count>2){
                        temp.syntax_error = "Error: Extraneous text in command or after end of command";
                        free_line_disassembly(&new_line);
                        return temp;
                    }
                    /* Handle .extern directive */
                    if (!is_lable(new_line.strings[i])) {
                        temp.syntax_error = "Error: invalid label name";
                        free_line_disassembly(&new_line);
                        return temp;
                    }
                    temp.option.is_dir.dir_type = exter;
                    temp.option.is_dir.dir_data.label_entr_extr = malloc(80 * sizeof(char));
                    strcpy(temp.option.is_dir.dir_data.label_entr_extr, new_line.strings[i]);
                    break;
                default:
                    temp.syntax_error = "Error: invalid directive";
            }
            break;
        case instruction:
            /* Handle instructions */
            temp.type_of_line = instruction;
            inst = instruction_type(new_line.strings[i++], &temp);
            switch (inst) {
                case lea:
                case mov:
                case add:
                case sub:
                case cmp:
                    /* Handle two-operand instructions */
                    if (new_line.strings_count > 4) {
                        temp.syntax_error = "Error: Extraneous text in command or after end of command";
                        free_line_disassembly(&new_line);
                        return temp;
                    }
                    for (j = 0; j < 3; j++, ++i) {
                        if (j == 1) {
                            if (new_line.strings[i] == NULL || strcmp(new_line.strings[i], ",") != 0) {
                                temp.syntax_error = "Error: missing comma";
                                free_line_disassembly(&new_line);
                                return temp;
                            }
                            continue;
                        }
                        addr = handel_addressing(new_line.strings[i], &temp, (j == 2) ? 1 : 0);
                        if (addr == 1) {
                            temp.syntax_error = "Error: Undefined operand";
                            free_line_disassembly(&new_line);
                            return temp;
                        }
                        if (addr == -1) {
                            free_line_disassembly(&new_line);
                            return temp;
                        }
                    }
                    if (inst == lea) {
                        if (temp.option.is_inst.op[0].addr_method != direct) {
                            temp.syntax_error = "Error: invalid addressing method";
                            free_line_disassembly(&new_line);
                            return temp;
                        }
                    }
                    if (inst == mov || inst == add || inst == sub || inst == lea) {
                        if (temp.option.is_inst.op[1].addr_method == immediate) {
                            temp.syntax_error = "Error: invalid addressing method";
                            free_line_disassembly(&new_line);
                            return temp;
                        }
                    }
                    break;
                case not:
                case clr:
                case inc:
                case dec:
                case jmp:
                case bne:
                case red:
                case prn:
                case jsr:
                    /* Handle one-operand instructions */
                    if (new_line.strings_count != 2) {
                        temp.syntax_error = "Error: Extraneous text in command or after end of command";
                        free_line_disassembly(&new_line);
                        return temp;
                    }
                    j = handel_addressing(new_line.strings[i], &temp, 1);
                    if (j == 1) {
                        temp.syntax_error = "Error: Undefined label";
                        free_line_disassembly(&new_line);
                        return temp;
                    }
                    if(inst == jmp || inst == bne || inst == jsr){
                        if(temp.option.is_inst.op[1].addr_method == immediate || temp.option.is_inst.op[1].addr_method == direct_register){
                            temp.syntax_error = "Error: invalid addressing method";
                            free_line_disassembly(&new_line);
                            return temp;
                        }
                    }
                    if(inst == clr || inst == not || inst == inc || inst == dec || inst == red){
                        if(temp.option.is_inst.op[1].addr_method == immediate){
                            temp.syntax_error = "Error: invalid addressing method";
                            free_line_disassembly(&new_line);
                            return temp;
                        }
                    }
                    temp.option.is_inst.op[0].addr_method = no_operand;
                    break;
                case rts:
                case stop:
                    /* Handle zero-operand instructions */
                    if (new_line.strings_count != 1) {
                        temp.syntax_error = "Error: Extraneous text in command or after end of command";
                        free_line_disassembly(&new_line);
                        return temp;
                    }
                    temp.option.is_inst.op[0].addr_method = no_operand;
                    temp.option.is_inst.op[1].addr_method = no_operand;
                    break;
                default:
                    temp.syntax_error = "Error: invalid instruction";
            }
            break;
        default:
            if(new_line.strings_count == 1){
                temp.syntax_error = "Error: invalid instruction/directive";
            } else if (strcmp(new_line.strings[1], "mov") == 0 || strcmp(new_line.strings[1], "cmp") == 0 ||
                strcmp(new_line.strings[1], "add") == 0 || strcmp(new_line.strings[1], "sub") == 0 ||
                strcmp(new_line.strings[1], "lea") == 0) {
                temp.syntax_error = "Missing colon after label";

            } else{
                temp.syntax_error = "Error: invalid instruction/directive";
            }
    }
    free_line_disassembly(&new_line);
    return temp;
}


int handel_addressing(char *str, struct template *temp, int index) {
    /* Check for null input */
    if (str == NULL) {
        temp->syntax_error = "Error: missing operand";
        return -1;
    }

    /* Handle immediate addressing */
    if (str[0] == '#') {
        if (!is_number(str + 1)) {
            temp->syntax_error = "Error: invalid immediate addressing - not a valid number";
            return -1;
        }
        temp->option.is_inst.op[index].addr_method = immediate;
        temp->option.is_inst.op[index].op_addr_method.immediate = strtol(str + 1, NULL, 10);
        return 0;
    }

    /* Handle indirect register addressing */
    if (str[0] == '*') {
        if (str[1] != 'r' || str[2] < '0' || str[2] > '7' || str[3] != '\0') {
            temp->syntax_error = "Error: invalid indirect register addressing";
            return -1;
        }
        temp->option.is_inst.op[index].addr_method = indirect_register;
        temp->option.is_inst.op[index].op_addr_method.register_address = strtol(str + 2, NULL, 10);
        return 0;
    }

    /* Handle direct register addressing */
    if (str[0] == 'r' && strlen(str) == 2) {
        if (str[1] < '0' || str[1] > '7' || str[2] != '\0') {
            temp->syntax_error = "Error: invalid register name";
            return -1;
        }
        temp->option.is_inst.op[index].addr_method = direct_register;
        temp->option.is_inst.op[index].op_addr_method.register_address = strtol(str + 1, NULL, 10);
        return 0;
    }

    /* Handle direct addressing (label) */
    if (is_lable(str)) {
        temp->option.is_inst.op[index].addr_method = direct;
        temp->option.is_inst.op[index].op_addr_method.label = malloc(80 * sizeof(char));
        strcpy(temp->option.is_inst.op[index].op_addr_method.label, str);
        return 0;
    }
    return 1;
}

int is_lable(char *str) {
    int i;
    if (str[0] < 'a' || str[0] > 'z') {
        if (str[0] < 'A' || str[0] > 'Z') {
            return 0;
        }
    }
    for (i = 1; i < strlen(str); ++i) {
        if (str[i] < 'a' || str[i] > 'z') {
            if (str[i] < 'A' || str[i] > 'Z') {
                if (str[i] < '0' || str[i] > '9') {
                    return 0;
                }
            }
        }
    }
    if (strlen(str) > 31) {
        return 0;
    }
    return 1;
}

int is_lable_dec(char *str,struct template *temp, macro_table *mcr_table) {
    int i;
    if (str == NULL) { /* Check for null input */
        return 0;
    }
    if (str[strlen(str) - 1] != ':') { /* Check for colon at the end, if not - no label dec */
        return 2;
    }
    /*remove ":" from the end of str*/
    str[strlen(str) - 1] = '\0';
    /*chek if str is in mcr_table*/
    if(get_macro(mcr_table,str)){
        temp->syntax_error = "Error: label name is a macro name";
        return 0;
    }
    if (str[0] < 'a' || str[0] > 'z') { /* Check for valid first character */
        if (str[0] < 'A' || str[0] > 'Z') {
            temp->syntax_error = "Error: invalid label name, label name must start with a letter";
            return 0;
        }
    }
    for (i = 1; i < strlen(str)-1; ++i) { /* Check for valid characters */
        if (str[i] < 'a' || str[i] > 'z') {
            if (str[i] < 'A' || str[i] > 'Z') {
                if (str[i] < '0' || str[i] > '9') {
                    temp->syntax_error = "Error: invalid label name, label name must contain only letters and numbers";
                    return 0;
                }
            }
        }
    }
    if (strlen(str) > 31) {
        temp->syntax_error = "Error: invalid label name, label name must be less than 31 characters";
        return 0;
    }

    if (is_reserved_word(str)) {
        temp->syntax_error = "Error: label name is a reserved word";
        return 0;
    }
    return 1;
}


int line_type(char *str) {
    if (str[0] == '.') {
        return directive;
    }
    if (is_instruction(str)) {
        return instruction;
    }
    return -1;
}

int directive_type(char *str) {
    if (strcmp(str, ".data") == 0) {
        return data;
    }
    if (strcmp(str, ".string") == 0) {
        return string;
    }
    if (strcmp(str, ".entry") == 0) {
        return entry;
    }
    if (strcmp(str, ".extern") == 0) {
        return exter;
    }
    return -1;
}

int instruction_type(char *str, struct template *temp) {
    const char *instruction_names[] = {
            "mov", "cmp", "add", "sub",
            "lea", "clr", "not", "inc",
            "dec", "jmp", "bne", "red",
            "prn", "jsr", "rts", "stop"
    };
    int i;
    for (i = 0; i < 16; ++i) {
        if (strcmp(str, instruction_names[i]) == 0) {
            temp->option.is_inst.inst_type = i;
            return i;
        }
    }
    return -1;
}

int is_number(const char *str) {
    /* Check for empty string */
    if (*str == '\0') {
        return 0;
    }

    /* If the string starts with a minus sign, move to the next character */
    if (*str == '-') {
        str++;
        /* Ensure there is at least one digit after the minus sign */
        if (*str == '\0') {
            return 0;
        }
    }

    /* Check each character to ensure it is a digit */
    while (*str) {
        if (!isdigit((unsigned char) *str)) {
            return 0;
        }
        str++;
    }

    return 1;
}

struct template init_template() {
    struct template temp;
    temp.label.name = NULL;
    temp.type_of_line = empty;
    temp.option.is_dir.dir_data.data[0] = 0;
    temp.syntax_error = "\0";
    return temp;
}

void free_template(struct template *t)
{
    /* Declare variables at the beginning */
    int i;

    if (t == NULL)
    {
        return;
    }

    /* Free syntax_error if it exists */
    if (t->syntax_error != NULL)
    {
        free(t->syntax_error);
    }

    /* Free label name if it exists */
    if (t->label.name != NULL)
    {
        free(t->label.name);
    }

    /* Free content based on the type of line */
    switch (t->type_of_line)
    {
        case directive:
            switch (t->option.is_dir.dir_type)
            {
                case string:
                    if (t->option.is_dir.dir_data.string != NULL)
                    {
                        free(t->option.is_dir.dir_data.string);
                    }
                    break;
                case entry:
                case exter:
                    if (t->option.is_dir.dir_data.label_entr_extr != NULL)
                    {
                        free(t->option.is_dir.dir_data.label_entr_extr);
                    }
                    break;
                case data:
                    /* No need to free anything for data array */
                    break;
            }
            break;
        case instruction:
            /* Free labels in operands if they exist */
            for (i = 0; i < 2; i++)
            {
                if (t->option.is_inst.op[i].addr_method == direct &&
                    t->option.is_inst.op[i].op_addr_method.label != NULL)
                {
                    free(t->option.is_inst.op[i].op_addr_method.label);
                }
            }
            break;
        case empty:
            /* Nothing to free for empty lines */
            break;
    }

    /* Set all pointers to NULL to avoid double-free issues */
    t->syntax_error = NULL;
    t->label.name = NULL;
    if (t->type_of_line == directive)
    {
        t->option.is_dir.dir_data.string = NULL;
        t->option.is_dir.dir_data.label_entr_extr = NULL;
    }
    if (t->type_of_line == instruction)
    {
        t->option.is_inst.op[0].op_addr_method.label = NULL;
        t->option.is_inst.op[1].op_addr_method.label = NULL;
    }
}
