#include "macro_data_structures.h"


macro *add_macro(char *macro_name) {
    macro *new_macro = (macro *) malloc(sizeof(macro));
    if (new_macro == NULL) {
        printf("Error: Cannot allocate memory\n");
        exit(1);
    }
    new_macro->macro_name = (char *) malloc(strlen(macro_name) + 1);
    if (new_macro->macro_name == NULL) {
        printf("Error: Cannot allocate memory\n");
        exit(1);
    }
    strcpy(new_macro->macro_name, macro_name);
    new_macro->lines = NULL;
    new_macro->lines_count = 0;
    return new_macro;
}

macro *get_macro(macro_table *mcr_table, char *macro_name) {
    macro_table *temp = mcr_table;
    while (temp != NULL) {
        if (temp->macro == NULL) {
            temp = temp->next;
            continue;
        }
        if(temp->macro->macro_name == NULL){
            temp = temp->next;
            continue;
        }
        if (strncmp(temp->macro->macro_name, macro_name, strlen(macro_name)) == 0) {
            return temp->macro;
        }
        temp = temp->next;
    }
    return NULL;
}




void add_new_line(macro *mcr, char *line) {
    /*the lines shold be devided by \n*/
    char *new_lines;
    if (mcr->lines == NULL) {
        new_lines = (char *) malloc(LINE_BUFFER + 1);
        if (new_lines == NULL) {
            printf("Error: Cannot allocate memory\n");
            exit(1);
        }
        strcpy(new_lines, line);
        mcr->lines = new_lines;
        mcr->lines_count++;
        return;
    }
    new_lines = (char *) malloc(LINE_BUFFER + strlen(mcr->lines) + 2);
    if (new_lines == NULL) {
        printf("Error: Cannot allocate memory\n");
        exit(1);
    }
    strcpy(new_lines, mcr->lines);
    strcat(new_lines, line);
    free(mcr->lines);
    mcr->lines = new_lines;
    mcr->lines_count++;
}


void add_macro_table(macro_table *mcr_table, macro *mcr) {
    macro_table *temp = mcr_table;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = init_macro_table();
    temp->next->macro->macro_name = (char *) malloc(strlen(mcr->macro_name) + 1);
    if (temp->next->macro->macro_name == NULL) {
        printf("Error: Cannot allocate memory\n");
        exit(1);
    }
    strcpy(temp->next->macro->macro_name, mcr->macro_name);
    temp->next->macro->lines = (char *) malloc(strlen(mcr->lines) + 1);
    if (temp->next->macro->lines == NULL) {
        printf("Error: Cannot allocate memory\n");
        exit(1);
    }
    strcpy(temp->next->macro->lines, mcr->lines);
    temp->next->macro->lines_count = mcr->lines_count;
}

void free_macro_table(macro_table *mcr_table) {
    macro_table *temp;
    while (mcr_table != NULL) {
        temp = mcr_table;
        if (temp->macro != NULL) {
            free(temp->macro->macro_name);
            free(temp->macro->lines);
            free(temp->macro);
        }
        mcr_table = mcr_table->next;
    }
}

macro_table *init_macro_table() {
    macro_table *new_table = (macro_table *) malloc(sizeof(macro_table));
    if (new_table == NULL) {
        printf("Error: Cannot allocate memory\n");
        exit(1);
    }
    new_table->macro = init_macro();
    new_table->next = NULL;
    return new_table;
}
macro *init_macro() {
    macro *new_macro = (macro *) malloc(sizeof(macro));
    if (new_macro == NULL) {
        printf("Error: Cannot allocate memory\n");
        exit(1);
    }
    new_macro->macro_name = NULL;
    new_macro->lines = NULL;
    new_macro->lines_count = 0;
    return new_macro;
}