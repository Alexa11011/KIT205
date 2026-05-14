#include <stdio.h>
#include <stdlib.h>

#include "hash_for_avl.h"

AVLHashTable create_hash_for_avl(int n) {
    AVLHashTable new_table;

    new_table.size = n;
    new_table.table = malloc(sizeof(AVL) * n);

    if (new_table.table == NULL) {
        fprintf(stderr, "Error allocating hash table.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; i++) {
        new_table.table[i] = new_avl();
    }

    return new_table;
}

int hash_avl_key(int key, int size) {
    unsigned int value = (unsigned int) key;
    return (int) (value % (unsigned int) size);
}

void hash_avl_insert(AVLHashTable *self, int key) {
    int index = hash_avl_key(key, self->size);
    insert_avl(&(self->table[index]), key);
}

void hash_avl_remove(AVLHashTable *self, int key) {
    int index = hash_avl_key(key, self->size);
    delete_avl(&(self->table[index]), key);
}

void hash_avl_print(AVLHashTable *self) {
    for (int i = 0; i < self->size; i++) {
        printf("%d: ", i);
        print_in_order_avl(&(self->table[i]));
        printf("\n");
    }
}

void hash_avl_destroy(AVLHashTable *self) {
    for (int i = 0; i < self->size; i++) {
        destroy_avl(&(self->table[i]));
    }

    free(self->table);
    self->table = NULL;
    self->size = 0;
}
