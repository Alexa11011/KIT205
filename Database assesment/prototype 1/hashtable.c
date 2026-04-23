#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"

HashTable create_hashtable(int n) {
    HashTable new_table;

    new_table.size = n;
    new_table.table = malloc(sizeof(List) * n);

    if (new_table.table == NULL) {
        fprintf(stderr, "Error allocating hash table.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; i++) {
        new_table.table[i] = new_list();
    }

    return new_table;
}

int hash(String key, int size) {
    unsigned long h = 0;
    int n = (int) strlen(key);

    for (int i = 0; i < n; i++) {
        h = (h << 5) + (unsigned char) key[i];
    }

    return (int) (h % (unsigned long) size);
}

void hash_insert(HashTable *self, String key) {
    int index = hash(key, self->size);
    insert_at_front(&(self->table[index]), key);
}

void hash_remove(HashTable *self, String key) {
    int index = hash(key, self->size);
    delete_from_list(&(self->table[index]), key);
}

void hash_print(HashTable *self) {
    for (int i = 0; i < self->size; i++) {
        printf("%d: ", i);
        print_list(&(self->table[i]));
    }
}

void hash_destroy(HashTable *self) {
    for (int i = 0; i < self->size; i++) {
        destroy_list(&(self->table[i]));
    }

    free(self->table);
    self->table = NULL;
    self->size = 0;
}
