#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct label {
    char name[32];
    enum {
        is_entry,
        is_entry_data,
        is_entry_code,
        is_data,
        is_code,
        is_extern
    } label_type;
    int address;
    int def_line;
};

struct entry {
    struct label *label;
    int occupied;
};

struct label_table {
    struct entry *entries;
    int size;
    int count;
};

unsigned int hash(char *name, int size);

struct label_table *create_table(int size);

void free_table(struct label_table *table);

void resize(struct label_table *table);

void insert(struct label_table *table, struct label *label);

struct label *search(struct label_table *table, char *name);

void free_label(struct label *label);

/***************************************************************/


struct ent_ex {
    char *name;
    int *address;
};

struct translator {
    int *code_image;
    int IC;
    int *data_image;
    int DC;

    struct label_table *label_table;
    int label_count;

    struct ent_ex *externals;
    int externals_count;

    struct ent_ex *entries;
    int entries_count;
};

void add_entry(struct translator *translator, struct label *label);

void add_external(struct translator *translator, char *name, int ic);

void add_label(struct translator *translator, struct label *label);

void add_code_image(struct translator *trans, unsigned int code, int ic);

void add_data_image(struct translator *trans, int data, int dc);

int is_external(struct translator *trans, char *name);

struct translator *init_translator();

void free_translator(struct translator *t);