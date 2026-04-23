#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hashtable.h"

static void assert_bucket_equals(List *bucket, const char *expected[], int expected_count) {
    ListNodePtr current = bucket->head;

    for (int i = 0; i < expected_count; i++) {
        assert(current != NULL);
        assert(strcmp(current->data, expected[i]) == 0);
        current = current->next;
    }

    assert(current == NULL);
}

static char *capture_hash_print_output(HashTable *table) {
    int saved_stdout = dup(fileno(stdout));
    FILE *temp = tmpfile();

    assert(saved_stdout != -1);
    assert(temp != NULL);
    assert(dup2(fileno(temp), fileno(stdout)) != -1);

    hash_print(table);
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

static void test_create_hashtable_initialises_empty_buckets(void) {
    HashTable table = create_hashtable(11);

    assert(table.size == 11);
    assert(table.table != NULL);

    for (int i = 0; i < table.size; i++) {
        assert(table.table[i].head == NULL);
    }

    hash_destroy(&table);
}

static void test_hash_returns_expected_index_for_known_values(void) {
    assert(hash("apple", 11) == 2);
    assert(hash("banana", 11) == 6);
    assert(hash("pear", 11) == 6);
    assert(hash("grape", 11) == 9);
}

static void test_insert_places_items_in_expected_buckets(void) {
    HashTable table = create_hashtable(11);
    const char *apple_expected[] = {"apple"};
    const char *collision_expected[] = {"pear", "banana"};

    hash_insert(&table, "apple");
    hash_insert(&table, "banana");
    hash_insert(&table, "pear");

    assert_bucket_equals(&(table.table[2]), apple_expected, 1);
    assert_bucket_equals(&(table.table[6]), collision_expected, 2);

    hash_destroy(&table);
}

static void test_insert_stores_copy_of_input_string(void) {
    HashTable table = create_hashtable(11);
    char word[] = "kiwi";
    int index = hash(word, 11);

    hash_insert(&table, word);
    word[0] = 'K';

    assert(table.table[index].head != NULL);
    assert(strcmp(table.table[index].head->data, "kiwi") == 0);

    hash_destroy(&table);
}

static void test_remove_deletes_matching_item_from_collision_bucket(void) {
    HashTable table = create_hashtable(11);
    const char *expected[] = {"pear"};

    hash_insert(&table, "banana");
    hash_insert(&table, "pear");
    hash_remove(&table, "banana");

    assert_bucket_equals(&(table.table[6]), expected, 1);

    hash_destroy(&table);
}

static void test_remove_deletes_all_duplicate_values_in_bucket(void) {
    HashTable table = create_hashtable(11);

    hash_insert(&table, "pear");
    hash_insert(&table, "pear");
    hash_insert(&table, "pear");
    hash_remove(&table, "pear");

    assert(table.table[6].head == NULL);

    hash_destroy(&table);
}

static void test_remove_missing_value_leaves_table_unchanged(void) {
    HashTable table = create_hashtable(11);
    const char *expected[] = {"pear", "banana"};

    hash_insert(&table, "banana");
    hash_insert(&table, "pear");
    hash_remove(&table, "dragonfruit");

    assert_bucket_equals(&(table.table[6]), expected, 2);

    hash_destroy(&table);
}

static void test_hash_print_matches_expected_format(void) {
    HashTable table = create_hashtable(5);

    hash_insert(&table, "apple");
    hash_insert(&table, "banana");
    hash_insert(&table, "pear");

    char *output = capture_hash_print_output(&table);
    assert(strcmp(output, "0: \n1: \n2: \n3: pear, banana, apple\n4: \n") == 0);

    free(output);
    hash_destroy(&table);
}

static void test_hash_destroy_clears_table_state(void) {
    HashTable table = create_hashtable(11);

    hash_insert(&table, "apple");
    hash_insert(&table, "banana");
    hash_destroy(&table);

    assert(table.size == 0);
    assert(table.table == NULL);
}

int main(void) {
    test_create_hashtable_initialises_empty_buckets();
    test_hash_returns_expected_index_for_known_values();
    test_insert_places_items_in_expected_buckets();
    test_insert_stores_copy_of_input_string();
    test_remove_deletes_matching_item_from_collision_bucket();
    test_remove_deletes_all_duplicate_values_in_bucket();
    test_remove_missing_value_leaves_table_unchanged();
    test_hash_print_matches_expected_format();
    test_hash_destroy_clears_table_state();

    printf("All hash table tests passed.\n");
    return 0;
}
