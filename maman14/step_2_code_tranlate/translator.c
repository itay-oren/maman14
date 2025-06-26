#include "translator.h"

/* Enum for ARE (Absolute, Relocatable, External) flags */

#define ARE_A 4
#define ARE_R 2
#define ARE_E 1
#define LSb 1
#define DEST_OP_POS 3
#define SOURCE_OP_POS 6
#define OPCODE_POS 11
#define DATA_ADDRESS_POS 3
#define SOURCE_OP 0
#define DEST_OP 1
#define MAX_MEMORY 3996

/**
 * Check if an operand is external and add it to the externals list if so
 * @param line The template of the current line
 * @param program Pointer to the translator structure
 * @param index Index of the operand to check
 * @param ic Current instruction counter
 */
void if_external(struct template line, struct translator *program, int index, int ic) {
    struct label *label;
    if (line.option.is_inst.op[index].addr_method == direct) {
        label = search(program->label_table, line.option.is_inst.op[index].op_addr_method.label);
        if (label != NULL && label->label_type == is_extern) {
            add_external(program, line.option.is_inst.op[index].op_addr_method.label, ic);
        }
    }
}


int first_translate(struct translator *program, char *file_name, FILE *input,macro_table *mcr_table) {
    char line[LINE_SIZE];
    int i;
    int line_count = 1;
    int error = 0;
    struct label *label = NULL;
    struct template line_template;
    program->IC = 0;
    program->DC = 0;

    while (fgets(line, LINE_SIZE, input)) {
        if(program->IC + program->DC >= MAX_MEMORY){
            printf("in %s: line: %d Error: program size exceeds memory limit\n", file_name, line_count);
            error = 1;
            break;
        }
        line_template = get_line_template(line, mcr_table);

        /* Check for syntax errors */
        if (line_template.syntax_error != NULL) {

            printf("in %s: line: %d Syntax error: %s\n", file_name, line_count, line_template.syntax_error);
            line_count++;
            error = 1;
            continue;
        }
        /* Skip empty lines */
        if (line_template.type_of_line == empty) {
            if (line_template.label.name != NULL) {
                label = malloc(sizeof(struct label));
                strcpy(label->name, line_template.label.name);
                label->label_type = is_entry;
                label->def_line = line_count;
                add_label(program, label);
            }
            line_count++;
            continue;
        }



        /* Process labels */
        if (line_template.label.name != NULL) {
            switch (line_template.type_of_line) {
                case directive:
                    if (!(line_template.option.is_dir.dir_type == data ||
                          line_template.option.is_dir.dir_type == string)) {
                        break;
                    }
                case instruction:
                    /* Handle label for instruction or data/string directive */
                    label = search(program->label_table, line_template.label.name);
                    if (label != NULL) {
                        /* Label already exists */
                        if (label->label_type == is_entry) {
                            /* Update entry label */
                            label->def_line = line_count;
                            if (line_template.type_of_line == instruction) {
                                label->label_type = is_entry_code;
                                label->address = program->IC +100;
                            } else {
                                label->label_type = is_entry_data;
                                label->address = program->DC ;
                            }
                        } else {
                            /* Error: label redefinition */
                            printf("in %s: line: %d Error: label %s already exists\n", file_name, line_count,
                                   line_template.label.name);
                            error = 1;
                        }
                    } else {
                        /* Create new label */
                        label = malloc(sizeof(struct label));
                        strcpy(label->name, line_template.label.name);
                        if (line_template.type_of_line == instruction) {
                            label->label_type = is_code;
                            label->address = program->IC + 100;
                        } else {
                            label->label_type = is_data;
                            label->address = program->DC;
                        }
                        label->def_line = line_count;
                        add_label(program, label);
                    }
                    break;
                default:
                    break;
            }
        }

        /* Update IC or DC based on line type */
        if (line_template.type_of_line == instruction) {
            program->IC = ic_dc_calc(line_template, program->IC);
        } else if (line_template.type_of_line == directive && line_template.option.is_dir.dir_type != entry &&
                   line_template.option.is_dir.dir_type != exter) {
            program->DC = ic_dc_calc(line_template, program->DC);
        } else if (line_template.type_of_line == directive && line_template.option.is_dir.dir_type == entry) {
            /* Handle entry directive */
            label = search(program->label_table, line_template.option.is_dir.dir_data.label_entr_extr);
            if (label != NULL) {
                if (label->label_type == is_code) {
                    label->label_type = is_entry_code;
                } else if (label->label_type == is_data) {
                    label->label_type = is_entry_data;
                } else {
                    printf("in %s: line: %d Error: label %s already exists in line: %d\n", file_name, line_count,
                           line_template.label.name, label->def_line);
                    error = 1;
                }
            } else {
                /* Create new entry label */
                label = malloc(sizeof(struct label));
                strcpy(label->name, line_template.option.is_dir.dir_data.label_entr_extr);
                label->label_type = line_template.option.is_dir.dir_type;
                label->def_line = line_count;
                add_label(program, label);

            }
        } else if (line_template.type_of_line == directive && line_template.option.is_dir.dir_type == exter) {
            /* Handle external directive */
            label = malloc(sizeof(struct label));
            strcpy(label->name, line_template.option.is_dir.dir_data.label_entr_extr);
            label->label_type = is_extern;
            label->def_line = line_count;
            add_label(program, label);
        }
        line_count++;
        free_template(&line_template);
    }

    /* Final pass: check for undefined entry labels and update data label addresses */
    for (i = 0; i < program->label_table->size; i++) {
        if (program->label_table->entries[i].occupied) {
            if (program->label_table->entries[i].label->label_type == is_entry) {
                printf("in %s: line: %d Error: label -%s- is not defined\n", file_name,
                       program->label_table->entries[i].label->def_line,
                       program->label_table->entries[i].label->name);
                error = 1;
            } else if (program->label_table->entries[i].label->label_type == is_data ||
                       program->label_table->entries[i].label->label_type == is_entry_data) {
                program->label_table->entries[i].label->address += program->IC + 100;
            } else if (program->label_table->entries[i].label->label_type <= is_entry_code) {
                /* No action needed for code labels */
            }
        }
    }

    return error;
}




int second_translate(struct translator *program, char *file_name, FILE *input,macro_table *mcr_table) {
    char line[LINE_SIZE];
    int i;
    int ic;
    int word;
    int line_count = 0;
    int error = 0;
    int source_op, dest_op;
    struct label *label = NULL;
    struct template line_template;
    program->IC = 0;
    program->DC = 0;

    while (fgets(line, LINE_SIZE, input) != NULL) {
        line_template = get_line_template(line, mcr_table);
        if (line_template.type_of_line == empty) {
            line_count++;
            continue;
        }

        /* Process labels */
        if (line_template.label.name != NULL) {
            label = search(program->label_table, line_template.label.name);
            if (label != NULL) {
                if (label->label_type == is_entry_code || label->label_type == is_entry_data) {
                    add_entry(program, label);
                }
            }
        }

        /* Process instructions */
        if (line_template.type_of_line == instruction) {
            ic = program->IC;
            /* First code word: */
            word = ARE_A; /* toggle ARE_A in ARE field*/

            /* Set source operand addressing method */
            if (line_template.option.is_inst.op[SOURCE_OP].addr_method != no_operand) {
                ic = program->IC + 1;
                if_external(line_template, program, SOURCE_OP, ic);
                word |= LSb << line_template.option.is_inst.op[SOURCE_OP].addr_method;
            }

            /* Set destination operand addressing method */
            if (line_template.option.is_inst.op[DEST_OP].addr_method != no_operand) {
                ic = ic == program->IC ? program->IC + 1 : program->IC + 2;
                if_external(line_template, program, DEST_OP, ic);
                word |= LSb << (line_template.option.is_inst.op[DEST_OP].addr_method - 4);
            }

            /* Set opcode */
            word |= line_template.option.is_inst.inst_type << OPCODE_POS;
            add_code_image(program, word, program->IC++);

            /* Other code words: */
            source_op = line_template.option.is_inst.op[SOURCE_OP].addr_method;
            dest_op = line_template.option.is_inst.op[DEST_OP].addr_method;

            /* Case of two register operands */
            if ((source_op == indirect_register || source_op == direct_register) &&
                (dest_op == indirect_register || dest_op == direct_register)) {
                word = ARE_A; /* toggle ARE_A in ARE field*/
                word |= line_template.option.is_inst.op[SOURCE_OP].op_addr_method.register_address << SOURCE_OP_POS;
                word |= line_template.option.is_inst.op[DEST_OP].op_addr_method.register_address << DEST_OP_POS;
                add_code_image(program, word, program->IC++);
            } else {
                /* Process each operand */
                for (i = 0; i < 2; ++i) {
                    source_op = line_template.option.is_inst.op[i].addr_method;
                    if (source_op == no_operand) {
                        continue;
                    }
                    if (source_op == immediate) {
                        word = ARE_A; /* toggle ARE_A in ARE field*/
                        word |= line_template.option.is_inst.op[i].op_addr_method.immediate << ((i == SOURCE_OP) ? SOURCE_OP_POS : DEST_OP_POS);
                        add_code_image(program, word, program->IC++);
                    } else if (source_op == direct) {
                        label = search(program->label_table,
                                       line_template.option.is_inst.op[i].op_addr_method.label);
                        if (label == NULL) {
                            printf("in %s: line: %d Error: label %s not found\n", file_name, line_count,
                                   line_template.option.is_inst.op[i].op_addr_method.label);
                            error = 1;
                            continue;
                        }
                        if(is_external(program, label->name)){
                            word = ARE_E; /* toggle E in ARE field*/
                        } else{
                            word = ARE_R; /* toggle R in ARE field*/
                            word |= (label->address) << DATA_ADDRESS_POS;
                        }
                        add_code_image(program, word, program->IC++);
                    } else if (source_op == indirect_register || source_op == direct_register) {
                        word = ARE_A; /* toggle ARE_A in ARE field*/
                        word |=  (i == 0) ? line_template.option.is_inst.op[0].op_addr_method.register_address << SOURCE_OP_POS :
                                line_template.option.is_inst.op[1].op_addr_method.register_address << DEST_OP_POS;
                        add_code_image(program, word, program->IC++);
                    } else {
                        printf("in %s: line: %d Error: invalid addressing method\n", file_name, line_count);
                        error = 1;
                    }
                }
            }
        }
            /* Process directives */
        else if (line_template.type_of_line == directive) {
            if (line_template.option.is_dir.dir_type == data) {
                for (i = 1; i <= line_template.option.is_dir.dir_data.data[0]; ++i) {
                    word = line_template.option.is_dir.dir_data.data[i];
                    add_data_image(program, word, program->DC++);
                }
            } else if (line_template.option.is_dir.dir_type == string) {
                for (i = 0; i < strlen(line_template.option.is_dir.dir_data.string); ++i) {
                    word = (int) line_template.option.is_dir.dir_data.string[i];
                    add_data_image(program, word, program->DC++);
                }
                add_data_image(program, 0, program->DC++);
            }
        }
        line_count++;
        free_template(&line_template);
    }

    return error;
}


int ic_dc_calc(struct template line, int current) {
    int result = 0;
    int op_1 = line.option.is_inst.op[0].addr_method;
    int op_2 = line.option.is_inst.op[1].addr_method;

    if (line.type_of_line == instruction) {
        /* Case 1: Two register operands */
        if ((op_1 == indirect_register || op_1 == direct_register) &&
            (op_2 == indirect_register || op_2 == direct_register)) {
            result += 2;
        }
            /* Case 2: Two-operand instructions (mov, cmp, add, sub, lea) */
        else if (line.option.is_inst.inst_type == mov || line.option.is_inst.inst_type == cmp ||
                 line.option.is_inst.inst_type == add || line.option.is_inst.inst_type == sub ||
                 line.option.is_inst.inst_type == lea) {
            result += 3;
        }
            /* Case 3: One-operand instructions (not, clr, inc, dec, jmp, bne, red, prn, jsr) */
        else if (line.option.is_inst.inst_type == not || line.option.is_inst.inst_type == clr ||
                 line.option.is_inst.inst_type == inc || line.option.is_inst.inst_type == dec ||
                 line.option.is_inst.inst_type == jmp || line.option.is_inst.inst_type == bne ||
                 line.option.is_inst.inst_type == red || line.option.is_inst.inst_type == prn ||
                 line.option.is_inst.inst_type == jsr) {
            result += 2;
        }
            /* Case 4: Zero-operand instructions */
        else {
            result++;
        }
    } else if (line.type_of_line == directive) {
        /* Case 5: Data directive */
        if (line.option.is_dir.dir_type == data) {
            result += line.option.is_dir.dir_data.data[0];
        }
            /* Case 6: String directive */
        else if (line.option.is_dir.dir_type == string) {
            result += strlen(line.option.is_dir.dir_data.string) + 1; /* +1 for null terminator */
        }
    }

    return result + current;
}