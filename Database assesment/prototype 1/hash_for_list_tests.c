#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hash_for_list.h"
#include "../tests.h"

static void assert_bucket_equals(List *bucket, const int expected[], int expected_count) {
    ListNodePtr current = bucket->head;

    for (int i = 0; i < expected_count; i++) {
        assert(current != NULL);
        assert(current->data == expected[i]);
        current = current->next;
    }

    assert(current == NULL);
}

static char *capture_hash_list_print_output(HashTable *table) {
    int saved_stdout = dup(fileno(stdout));
    FILE *temp = tmpfile();

    assert(saved_stdout != -1);
    assert(temp != NULL);
    assert(dup2(fileno(temp), fileno(stdout)) != -1);

    hash_list_print(table);
    fflush(stdout);

    assert(dup2(saved_stdout, fileno(stdout)) != -1);
    close(saved_stdout);

    assert(fseek(temp, 0, SEEK_END) == 0);
    long length = ftell(temp);
    assert(length >= 0);
    rewind(temp);

    char *buffer = malloc((size_t) length + 1);
    assert(buffer != NULL);
    assert(fread(buffer, 1, (size_t) length, temp) == (size_t) length);
    buffer[length] = '\0';

    fclose(temp);
    return buffer;
}

static void test_create_hash_for_list_initialises_empty_buckets(void) {
    HashTable table = create_hash_for_list(11);

    assert(table.size == 11);
    assert(table.table != NULL);

    for (int i = 0; i < table.size; i++) {
        assert(table.table[i].head == NULL);
    }

    hash_list_destroy(&table);
}

static void test_hash_returns_expected_index_for_known_values(void) {
    assert(hash_list_key(10, 11) == 10);
    assert(hash_list_key(11, 11) == 0);
    assert(hash_list_key(22, 11) == 0);
    assert(hash_list_key(35, 11) == 2);
}

static void test_insert_places_items_in_expected_buckets(void) {
    HashTable table = create_hash_for_list(11);
    int ten_expected[] = {10};
    int collision_expected[] = {22, 11};

    hash_list_insert(&table, 10);
    hash_list_insert(&table, 11);
    hash_list_insert(&table, 22);

    assert_bucket_equals(&(table.table[10]), ten_expected, 1);
    assert_bucket_equals(&(table.table[0]), collision_expected, 2);

    hash_list_destroy(&table);
}

static void test_insert_stores_integer_value(void) {
    HashTable table = create_hash_for_list(11);
    int index = hash_list_key(42, 11);

    hash_list_insert(&table, 42);

    assert(table.table[index].head != NULL);
    assert(table.table[index].head->data == 42);

    hash_list_destroy(&table);
}

static void test_remove_deletes_matching_item_from_collision_bucket(void) {
    HashTable table = create_hash_for_list(11);
    int expected[] = {22};

    hash_list_insert(&table, 11);
    hash_list_insert(&table, 22);
    hash_list_remove(&table, 11);

    assert_bucket_equals(&(table.table[0]), expected, 1);

    hash_list_destroy(&table);
}

static void test_remove_deletes_all_duplicate_values_in_bucket(void) {
    HashTable table = create_hash_for_list(11);

    hash_list_insert(&table, 22);
    hash_list_insert(&table, 22);
    hash_list_insert(&table, 22);
    hash_list_remove(&table, 22);

    assert(table.table[0].head == NULL);

    hash_list_destroy(&table);
}

static void test_remove_missing_value_leaves_table_unchanged(void) {
    HashTable table = create_hash_for_list(11);
    int expected[] = {22, 11};

    hash_list_insert(&table, 11);
    hash_list_insert(&table, 22);
    hash_list_remove(&table, 33);

    assert_bucket_equals(&(table.table[0]), expected, 2);

    hash_list_destroy(&table);
}

static void test_hash_list_print_matches_expected_format(void) {
    HashTable table = create_hash_for_list(5);

    hash_list_insert(&table, 3);
    hash_list_insert(&table, 8);
    hash_list_insert(&table, 13);

    char *output = capture_hash_list_print_output(&table);
    assert(strcmp(output, "0: \n1: \n2: \n3: 13, 8, 3\n4: \n") == 0);

    free(output);
    hash_list_destroy(&table);
}

static void test_hash_list_destroy_clears_table_state(void) {
    HashTable table = create_hash_for_list(11);

    hash_list_insert(&table, 11);
    hash_list_insert(&table, 22);
    hash_list_destroy(&table);

    assert(table.size == 0);
    assert(table.table == NULL);
}

void run_hash_for_list_tests(void) {
    test_create_hash_for_list_initialises_empty_buckets();
    test_hash_returns_expected_index_for_known_values();
    test_insert_places_items_in_expected_buckets();
    test_insert_stores_integer_value();
    test_remove_deletes_matching_item_from_collision_bucket();
    test_remove_deletes_all_duplicate_values_in_bucket();
    test_remove_missing_value_leaves_table_unchanged();
    test_hash_list_print_matches_expected_format();
    test_hash_list_destroy_clears_table_state();
}
