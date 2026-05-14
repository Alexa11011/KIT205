#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hash_for_avl.h"
#include "../tests.h"

static void assert_bucket_contains(AVL *bucket, int value) {
    assert(find_avl(bucket, value) != NULL);
}

static void assert_bucket_missing(AVL *bucket, int value) {
    assert(find_avl(bucket, value) == NULL);
}

static char *capture_hash_avl_print_output(AVLHashTable *table) {
    int saved_stdout = dup(fileno(stdout));
    FILE *temp = tmpfile();

    assert(saved_stdout != -1);
    assert(temp != NULL);
    assert(dup2(fileno(temp), fileno(stdout)) != -1);

    hash_avl_print(table);
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

static void test_create_hash_for_avl_initialises_empty_buckets(void) {
    AVLHashTable table = create_hash_for_avl(11);

    assert(table.size == 11);
    assert(table.table != NULL);

    for (int i = 0; i < table.size; i++) {
        assert(table.table[i].root == NULL);
    }

    hash_avl_destroy(&table);
}

static void test_hash_returns_expected_index_for_known_values(void) {
    assert(hash_avl_key(10, 11) == 10);
    assert(hash_avl_key(11, 11) == 0);
    assert(hash_avl_key(22, 11) == 0);
    assert(hash_avl_key(35, 11) == 2);
}

static void test_insert_places_items_in_expected_buckets(void) {
    AVLHashTable table = create_hash_for_avl(11);

    hash_avl_insert(&table, 10);
    hash_avl_insert(&table, 11);
    hash_avl_insert(&table, 22);

    assert_bucket_contains(&(table.table[10]), 10);
    assert_bucket_contains(&(table.table[0]), 11);
    assert_bucket_contains(&(table.table[0]), 22);
    assert_bucket_missing(&(table.table[1]), 22);

    hash_avl_destroy(&table);
}

static void test_insert_keeps_collision_bucket_as_avl(void) {
    AVLHashTable table = create_hash_for_avl(11);

    hash_avl_insert(&table, 33);
    hash_avl_insert(&table, 22);
    hash_avl_insert(&table, 11);

    assert(table.table[0].root != NULL);
    assert(table.table[0].root->data_item == 22);
    assert(table.table[0].root->left->data_item == 11);
    assert(table.table[0].root->right->data_item == 33);

    hash_avl_destroy(&table);
}

static void test_remove_deletes_matching_item_from_collision_bucket(void) {
    AVLHashTable table = create_hash_for_avl(11);

    hash_avl_insert(&table, 11);
    hash_avl_insert(&table, 22);
    hash_avl_remove(&table, 11);

    assert_bucket_missing(&(table.table[0]), 11);
    assert_bucket_contains(&(table.table[0]), 22);

    hash_avl_destroy(&table);
}

static void test_insert_ignores_duplicate_values(void) {
    AVLHashTable table = create_hash_for_avl(11);

    hash_avl_insert(&table, 22);
    hash_avl_insert(&table, 22);
    hash_avl_insert(&table, 22);

    assert(table.table[0].root != NULL);
    assert(table.table[0].root->data_item == 22);
    assert(table.table[0].root->left == NULL);
    assert(table.table[0].root->right == NULL);

    hash_avl_destroy(&table);
}

static void test_remove_missing_value_leaves_table_unchanged(void) {
    AVLHashTable table = create_hash_for_avl(11);

    hash_avl_insert(&table, 11);
    hash_avl_insert(&table, 22);
    hash_avl_remove(&table, 33);

    assert_bucket_contains(&(table.table[0]), 11);
    assert_bucket_contains(&(table.table[0]), 22);

    hash_avl_destroy(&table);
}

static void test_hash_avl_print_matches_expected_format(void) {
    AVLHashTable table = create_hash_for_avl(5);
    char *output;

    hash_avl_insert(&table, 3);
    hash_avl_insert(&table, 8);
    hash_avl_insert(&table, 13);

    output = capture_hash_avl_print_output(&table);
    assert(strcmp(output, "0: _\n1: _\n2: _\n3: ((_ 3 _) 8 (_ 13 _))\n4: _\n") == 0);

    free(output);
    hash_avl_destroy(&table);
}

static void test_hash_avl_destroy_clears_table_state(void) {
    AVLHashTable table = create_hash_for_avl(11);

    hash_avl_insert(&table, 11);
    hash_avl_insert(&table, 22);
    hash_avl_destroy(&table);

    assert(table.size == 0);
    assert(table.table == NULL);
}

void run_hash_for_avl_tests(void) {
    test_create_hash_for_avl_initialises_empty_buckets();
    test_hash_returns_expected_index_for_known_values();
    test_insert_places_items_in_expected_buckets();
    test_insert_keeps_collision_bucket_as_avl();
    test_remove_deletes_matching_item_from_collision_bucket();
    test_insert_ignores_duplicate_values();
    test_remove_missing_value_leaves_table_unchanged();
    test_hash_avl_print_matches_expected_format();
    test_hash_avl_destroy_clears_table_state();
}
