#include "tabel.h"

/* Hash function */
unsigned int hash(char *name, int size) {
    unsigned int hash_val = 0;
    while (*name) {
        hash_val = (hash_val << 5) + *name++;
    }
    return hash_val % size;
}

/* Create a new hash table*/
struct label_table *create_table(int size) {
    struct label_table *table = malloc(sizeof(struct label_table));
    if (table == NULL) {
        printf("Memory allocation failed for table.\n");
        exit(1);
    }
    table->size = size;
    table->count = 0;
    table->entries = calloc(size, sizeof(struct entry));
    if (table->entries == NULL) {
        printf("Memory allocation failed for table entries.\n");
        exit(1);
    }
    return table;
}

/* Free the hash table*/
void free_table(struct label_table *table) {
    free(table->entries);
    free(table);
}

/* Resize the hash table */
void resize(struct label_table *table) {
    unsigned int new_index;
    int i;
    int new_size = table->size + 1;
    struct entry *new_entries = calloc(new_size, sizeof(struct entry));
    if (new_entries == NULL) {
        printf("Memory allocation failed for new entries.\n");
        exit(1);
    }

    for (i = 0; i < table->size; i++) {
        if (table->entries[i].occupied) {
            new_index = hash(table->entries[i].label->name, new_size);
            while (new_entries[new_index].occupied) {
                new_index = (new_index + 1) % new_size;
            }
            new_entries[new_index] = table->entries[i];
        }
    }

    free(table->entries);
    table->entries = new_entries;
    table->size = new_size;
}

/* Insert a label into the hash table */
void insert(struct label_table *table, struct label *label) {
    unsigned int index;

    if (table->count == table->size) {
        resize(table);
    }

    index = hash(label->name, table->size);
    while (table->entries[index].occupied) {
        index = (index + 1) % table->size;
    }

    table->entries[index].label = label;
    table->entries[index].occupied = 1;
    table->count++;
}

/* Search for a label in the hash table */
/* Search for a label in the hash table */
struct label *search(struct label_table *table, char *name) {
    unsigned int index;
    int checkedEntries;
    if (table == NULL) {
        fprintf(stderr, "Error: Table is NULL\n");
        return NULL;
    }

    index = hash(name, table->size);
    checkedEntries = 0; /* Counter to track the number of entries checked */

    while (table->entries[index].occupied && checkedEntries < table->size) {
        if (strcmp(table->entries[index].label->name, name) == 0) {
            return table->entries[index].label;
        }
        index = (index + 1) % table->size;
        checkedEntries++; /* Increment the counter */
    }
    return NULL; /* Return NULL if the label is not found */
}


/* add a label to the translator */
void add_label(struct translator *translator, struct label *label) {
    insert(translator->label_table, label);
    translator->label_count++;
}

/* add an external to the translator */
void add_external(struct translator *trans, char *name, int ic) {
    /* Allocate or reallocate memory for the externals array */
    trans->externals = realloc(trans->externals, (trans->externals_count + 1) * sizeof(struct ent_ex));
    if (!trans->externals) {
        /* Handle memory allocation failure */
        printf("Memory allocation failed for externals array.\n");
        return;
    }

    /* Initialize the new external */
    trans->externals[trans->externals_count].name = malloc(strlen(name));
    strcpy(trans->externals[trans->externals_count].name, name);
    if (!trans->externals[trans->externals_count].name) {
        /* Handle memory allocation failure */
        printf("Memory allocation failed for external name.\n");
        exit(1);
    }

    /* Allocate memory for the address and set it to the current IC */
    trans->externals[trans->externals_count].address = malloc(sizeof(int));
    if (!trans->externals[trans->externals_count].address) {
        /* Handle memory allocation failure */
        printf("Memory allocation failed for external address.\n");
        exit(1);
    }
    *(trans->externals[trans->externals_count].address) = ic;

    /* Increment the count of externals */
    trans->externals_count++;
}

void add_entry(struct translator *trans, struct label *label) {
    /* Allocate or reallocate memory for the entries array */
    trans->entries = realloc(trans->entries, (trans->entries_count + 1) * sizeof(struct ent_ex));
    if (!trans->entries) {
        /* Handle memory allocation failure */
        printf("Memory allocation failed for entries array.\n");
        return;
    }

    /* Initialize the new entry */
    trans->entries[trans->entries_count].name = malloc(strlen(label->name));
    strcpy(trans->entries[trans->entries_count].name, label->name);
    if (!trans->entries[trans->entries_count].name) {
        /* Handle memory allocation failure */
        printf("Memory allocation failed for entry name.\n");
        exit(1);
    }

    /* Allocate memory for the address and set it to the current IC */
    trans->entries[trans->entries_count].address = malloc(sizeof(int));
    if (!trans->entries[trans->entries_count].address) {
        /* Handle memory allocation failure */
        printf("Memory allocation failed for entry address.\n");
        exit(1);
    }
    *(trans->entries[trans->entries_count].address) =
            label->label_type == is_entry_code ? trans->IC : trans->DC + trans->IC;

    /* Increment the count of entries */
    trans->entries_count++;
}

/* Function to add a code image */
void add_code_image(struct translator *trans, unsigned int code, int ic) {
    /* Allocate or reallocate memory for the code_image array */
    trans->code_image = realloc(trans->code_image, (ic + 1) * sizeof(int));

    /* Add the new code to the code_image array */
    trans->code_image[ic] = code;
}

/* Function to add a data image */
void add_data_image(struct translator *trans, int data, int dc) {
    /* Allocate or reallocate memory for the data_image array */
    trans->data_image = realloc(trans->data_image, (dc + 1) * sizeof(int));

    /* Add the new data to the data_image array */
    trans->data_image[dc] = data;
}

int is_external(struct translator *trans, char *name) {
    int i;
    for (i = 0; i < trans->externals_count; i++) {
        if (strcmp(trans->externals[i].name, name) == 0) {
            return 1;
        }
    }
    return 0;
}

void free_label(struct label *label) {
    if (label != NULL) {
        free(label);
    }
}

struct translator *init_translator(void) {
    struct translator *t;

    t = (struct translator *) malloc(sizeof(struct translator));
    if (t == NULL) {
        printf("Memory allocation failed");/* Memory allocation failed */
        exit(1);
    }

    t->code_image = NULL;
    t->IC = 0;
    t->data_image = NULL;
    t->DC = 0;
    t->label_table = create_table(1);
    t->label_count = 0;
    t->externals = NULL;
    t->externals_count = 0;
    t->entries = NULL;
    t->entries_count = 0;

    return t;
}

void free_translator(struct translator *t) {
    int i;

    if (t == NULL) {
        return;
    }

    if (t->code_image != NULL) {
        free(t->code_image);
    }

    if (t->data_image != NULL) {
        free(t->data_image);
    }

    if (t->label_table != NULL) {
        for (i = 0; i < t->label_table->size; i++) {
            if (t->label_table->entries[i].occupied) {
                free_label(t->label_table->entries[i].label);
            }
        }
        free_table(t->label_table);
    }

    if (t->externals != NULL) {
        for (i = 0; i < t->externals_count; i++) {
            if (t->externals[i].name != NULL) {
                free(t->externals[i].name);
            }
            if (t->externals[i].address != NULL) {
                free(t->externals[i].address);
            }
        }
        free(t->externals);
    }

    if (t->entries != NULL) {
        for (i = 0; i < t->entries_count; i++) {
            if (t->entries[i].name != NULL) {
                free(t->entries[i].name);
            }
            if (t->entries[i].address != NULL) {
                free(t->entries[i].address);
            }
        }
        free(t->entries);
    }

    free(t);
}
