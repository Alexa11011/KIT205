#pragma once

#include "avl.h"

typedef struct hashTable {
    int size;
    AVL *table;
} AVLHashTable;

AVLHashTable create_hash_for_avl(int n);
int hash_avl_key(int key, int size);
void hash_avl_insert(AVLHashTable *self, int key);
void hash_avl_remove(AVLHashTable *self, int key);
void hash_avl_print(AVLHashTable *self);
void hash_avl_destroy(AVLHashTable *self);
