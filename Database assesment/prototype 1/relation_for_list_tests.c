#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "relation_for_list.h"
#include "../tests.h"

static char *capture_list_relation_output(ListManyToManyRelation *relation, int id, int search_products) {
    int saved_stdout = dup(fileno(stdout));
    FILE *temp = tmpfile();

    assert(saved_stdout != -1);
    assert(temp != NULL);
    assert(dup2(fileno(temp), fileno(stdout)) != -1);

    if (search_products) {
        print_list_products_for_customer(relation, id);
    } else {
        print_list_customers_for_product(relation, id);
    }

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

static void test_create_list_relation_starts_empty(void) {
    ListManyToManyRelation relation = create_list_relation(11, 11);
    char *output = capture_list_relation_output(&relation, 150, 1);

    assert(strcmp(output, "Products for customer 150: none\n") == 0);

    free(output);
    destroy_list_relation(&relation);
}

static void test_insert_list_relationship_builds_both_directions(void) {
    ListManyToManyRelation relation = create_list_relation(11, 11);
    char *products_output;
    char *customers_output;

    insert_list_relationship(&relation, 150, 156);

    products_output = capture_list_relation_output(&relation, 150, 1);
    customers_output = capture_list_relation_output(&relation, 156, 0);

    assert(strcmp(products_output, "Products for customer 150: 156\n") == 0);
    assert(strcmp(customers_output, "Customers for product 156: 150\n") == 0);

    free(products_output);
    free(customers_output);
    destroy_list_relation(&relation);
}

static void test_many_to_many_list_relationships_are_sorted_on_read(void) {
    ListManyToManyRelation relation = create_list_relation(11, 11);
    char *products_output;
    char *customers_output;

    insert_list_relationship(&relation, 150, 300);
    insert_list_relationship(&relation, 150, 156);
    insert_list_relationship(&relation, 300, 156);
    insert_list_relationship(&relation, 300, 200);

    products_output = capture_list_relation_output(&relation, 150, 1);
    customers_output = capture_list_relation_output(&relation, 156, 0);

    assert(strcmp(products_output, "Products for customer 150: 156 300\n") == 0);
    assert(strcmp(customers_output, "Customers for product 156: 150 300\n") == 0);

    free(products_output);
    free(customers_output);
    destroy_list_relation(&relation);
}

static void test_duplicate_list_relationships_are_kept(void) {
    ListManyToManyRelation relation = create_list_relation(11, 11);
    char *products_output;
    char *customers_output;

    insert_list_relationship(&relation, 150, 156);
    insert_list_relationship(&relation, 150, 156);

    products_output = capture_list_relation_output(&relation, 150, 1);
    customers_output = capture_list_relation_output(&relation, 156, 0);

    assert(strcmp(products_output, "Products for customer 150: 156 156\n") == 0);
    assert(strcmp(customers_output, "Customers for product 156: 150 150\n") == 0);
    assert(count_list_relation_read_values(&relation) == 4);

    free(products_output);
    free(customers_output);
    destroy_list_relation(&relation);
}

static void test_load_list_relationships_from_file_builds_both_indexes(void) {
    const char *path = "/tmp/relation_for_list_test_data.txt";
    FILE *file;
    ListManyToManyRelation relation = create_list_relation(11, 11);
    int count;
    char *products_output;
    char *customers_output;

    file = fopen(path, "w");
    assert(file != NULL);

    fprintf(file, "150 156\n");
    fprintf(file, "150 200\n");
    fprintf(file, "300 156\n");
    fprintf(file, "150 156\n");
    fclose(file);

    count = load_list_relationships_from_file(&relation, path);
    products_output = capture_list_relation_output(&relation, 150, 1);
    customers_output = capture_list_relation_output(&relation, 156, 0);

    assert(count == 4);
    assert(strcmp(products_output, "Products for customer 150: 156 156 200\n") == 0);
    assert(strcmp(customers_output, "Customers for product 156: 150 150 300\n") == 0);

    free(products_output);
    free(customers_output);
    destroy_list_relation(&relation);
    unlink(path);
}

static void test_load_list_relationships_from_missing_file_returns_error(void) {
    ListManyToManyRelation relation = create_list_relation(11, 11);

    assert(load_list_relationships_from_file(&relation, "/tmp/does_not_exist_list_relationships.txt") == -1);

    destroy_list_relation(&relation);
}

void run_relation_for_list_tests(void) {
    test_create_list_relation_starts_empty();
    test_insert_list_relationship_builds_both_directions();
    test_many_to_many_list_relationships_are_sorted_on_read();
    test_duplicate_list_relationships_are_kept();
    test_load_list_relationships_from_file_builds_both_indexes();
    test_load_list_relationships_from_missing_file_returns_error();
}
