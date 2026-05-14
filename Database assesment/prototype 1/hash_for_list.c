#include <stdio.h>
#include <stdlib.h>

#include "hash_for_list.h"

HashTable create_hash_for_list(int n) {
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

int hash_list_key(int key, int size) {
    unsigned int value = (unsigned int) key;
    return (int) (value % (unsigned int) size);
}

void hash_list_insert(HashTable *self, int key) {
    int index = hash_list_key(key, self->size);
    insert_at_front(&(self->table[index]), key);
}

void hash_list_remove(HashTable *self, int key) {
    int index = hash_list_key(key, self->size);
    delete_from_list(&(self->table[index]), key);
}

void hash_list_print(HashTable *self) {
    for (int i = 0; i < self->size; i++) {
        printf("%d: ", i);
        print_list(&(self->table[i]));
    }
}

void hash_list_destroy(HashTable *self) {
    for (int i = 0; i < self->size; i++) {
        destroy_list(&(self->table[i]));
    }

    free(self->table);
    self->table = NULL;
    self->size = 0;
}
