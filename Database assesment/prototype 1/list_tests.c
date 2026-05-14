#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "list.h"
#include "../tests.h"

static void assert_list_equals(List *list, const int expected[], int expected_count) {
    ListNodePtr current = list->head;

    for (int i = 0; i < expected_count; i++) {
        assert(current != NULL);
        assert(current->data == expected[i]);
        current = current->next;
    }

    assert(current == NULL);
}

static char *capture_list_output(List *list) {
    int saved_stdout = dup(fileno(stdout));
    FILE *temp = tmpfile();

    assert(saved_stdout != -1);
    assert(temp != NULL);
    assert(dup2(fileno(temp), fileno(stdout)) != -1);

    print_list(list);
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

static void test_new_list_starts_empty(void) {
    List list = new_list();

    assert(list.head == NULL);
}

static void test_insert_at_front_adds_items_to_head(void) {
    List list = new_list();
    int expected[] = {30, 20, 10};

    insert_at_front(&list, 10);
    insert_at_front(&list, 20);
    insert_at_front(&list, 30);

    assert_list_equals(&list, expected, 3);

    destroy_list(&list);
}

static void test_insert_stores_integer_value(void) {
    List list = new_list();

    insert_at_front(&list, 42);

    assert(list.head != NULL);
    assert(list.head->data == 42);

    destroy_list(&list);
}

static void test_delete_from_list_removes_head_item(void) {
    List list = new_list();
    int expected[] = {20, 10};

    insert_at_front(&list, 10);
    insert_at_front(&list, 20);
    insert_at_front(&list, 30);
    delete_from_list(&list, 30);

    assert_list_equals(&list, expected, 2);

    destroy_list(&list);
}

static void test_delete_from_list_removes_middle_item(void) {
    List list = new_list();
    int expected[] = {30, 10};

    insert_at_front(&list, 10);
    insert_at_front(&list, 20);
    insert_at_front(&list, 30);
    delete_from_list(&list, 20);

    assert_list_equals(&list, expected, 2);

    destroy_list(&list);
}

static void test_delete_from_list_removes_all_duplicate_items(void) {
    List list = new_list();
    int expected[] = {20};

    insert_at_front(&list, 30);
    insert_at_front(&list, 20);
    insert_at_front(&list, 30);
    insert_at_front(&list, 30);
    delete_from_list(&list, 30);

    assert_list_equals(&list, expected, 1);

    destroy_list(&list);
}

static void test_delete_missing_item_leaves_list_unchanged(void) {
    List list = new_list();
    int expected[] = {30, 20, 10};

    insert_at_front(&list, 10);
    insert_at_front(&list, 20);
    insert_at_front(&list, 30);
    delete_from_list(&list, 99);

    assert_list_equals(&list, expected, 3);

    destroy_list(&list);
}

static void test_print_list_matches_expected_format(void) {
    List list = new_list();
    char *output;

    insert_at_front(&list, 10);
    insert_at_front(&list, 20);
    insert_at_front(&list, 30);

    output = capture_list_output(&list);
    assert(strcmp(output, "30, 20, 10\n") == 0);

    free(output);
    destroy_list(&list);
}

static void test_destroy_list_clears_head(void) {
    List list = new_list();

    insert_at_front(&list, 10);
    insert_at_front(&list, 20);
    destroy_list(&list);

    assert(list.head == NULL);
}

void run_list_tests(void) {
    test_new_list_starts_empty();
    test_insert_at_front_adds_items_to_head();
    test_insert_stores_integer_value();
    test_delete_from_list_removes_head_item();
    test_delete_from_list_removes_middle_item();
    test_delete_from_list_removes_all_duplicate_items();
    test_delete_missing_item_leaves_list_unchanged();
    test_print_list_matches_expected_format();
    test_destroy_list_clears_head();
}
