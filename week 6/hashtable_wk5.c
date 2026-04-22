// hashtable_wk5.c
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable_wk5.h"

HashTable create_hashtable(int n){
	HashTable newtable;

	newtable.size = n;
	newtable.table = malloc(sizeof(List) * n);

	for (int i = 0; i < n; i++) {
		newtable.table[i] = new_list();
	}

	return newtable;
}

int hash(String key, int size){
	unsigned long h = 0; // unsigned long, so that when it wraps it's still positive
	int n = strlen(key);

	for (int i = 0; i < n; i++){
		// multiply by 32 (<<5) and add ascii value
		h = (h << 5) + (int)(key[i]);
	}
	return h%size;
}

void hash_insert(HashTable *self, String key){
	// 1. find the list to insert into using hash
	// 2. call list function to insert into that list
	int index = hash(key, self->size);
	insert_at_front(&(self->table[index]), key);
}

void hash_remove(HashTable *self, String key){
	int index = hash(key, self->size);
	delete_from_list(&(self->table[index]), key);
}

void hash_print(HashTable *self){
	for (int i = 0; i < self->size; i++){
		printf("%d: ", i);
		print_list(&(self->table[i]));
	}
}

void hash_destroy(HashTable *self){
	for (int i = 0; i < self->size; i++) {
		destroy_list(&(self->table[i]));
	}

	free(self->table);
	self->table = NULL;
	self->size = 0;
}

static void assert_bucket_equals(List *bucket, String expected[], int expected_count) {
	ListNodePtr current = bucket->head;

	for (int i = 0; i < expected_count; i++) {
		assert(current != NULL);
		assert(strcmp(current->data, expected[i]) == 0);
		current = current->next;
	}

	assert(current == NULL);
}

void hash_adhoc_test(){
	int command = 1;
	HashTable table = create_hashtable(11);
	char buffer[100];

	while (command){
		scanf("%d", &command);
		switch (command){
		case 1:
			scanf("%99s", buffer);
			hash_insert(&table, buffer);
			break;
		case 2:
			scanf("%99s", buffer);
			hash_remove(&table, buffer);
			break;
		case 3:
			hash_print(&table);
			break;
		}
		printf("\n");
	}
	hash_destroy(&table);
}

void hash_test() {
	HashTable table = create_hashtable(11);

	assert(table.size == 11);
	for (int i = 0; i < table.size; i++) {
		assert(table.table[i].head == NULL);
	}

	hash_insert(&table, "apple");
	hash_insert(&table, "banana");
	hash_insert(&table, "pear");

	int apple_index = hash("apple", table.size);
	int banana_index = hash("banana", table.size);
	int pear_index = hash("pear", table.size);

	String apple_expected[] = {"apple"};
	String collision_expected[] = {"pear", "banana"};
	String pear_expected[] = {"pear"};

	assert_bucket_equals(&(table.table[apple_index]), apple_expected, 1);
	assert_bucket_equals(&(table.table[banana_index]), collision_expected, 2);
	assert(pear_index == banana_index);

	hash_remove(&table, "banana");
	assert_bucket_equals(&(table.table[banana_index]), pear_expected, 1);

	hash_remove(&table, "missing");
	assert_bucket_equals(&(table.table[apple_index]), apple_expected, 1);
	assert_bucket_equals(&(table.table[pear_index]), pear_expected, 1);

	hash_destroy(&table);
	assert(table.table == NULL);
	assert(table.size == 0);

	printf("hash_test passed\n");
}
