#pragma once

#include "list.h"

typedef struct hashTable {
    int size;
    List *table;
} HashTable;

HashTable create_hash_for_list(int n);
int hash_list_key(int key, int size);
void hash_list_insert(HashTable *self, int key);
void hash_list_remove(HashTable *self, int key);
void hash_list_print(HashTable *self);
void hash_list_destroy(HashTable *self);
