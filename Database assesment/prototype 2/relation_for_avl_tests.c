#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "relation_for_avl.h"
#include "../tests.h"

static void assert_tree_contains(AVL *tree, int value) {
    assert(tree != NULL);
    assert(find_avl(tree, value) != NULL);
}

static void assert_tree_missing(AVL *tree, int value) {
    assert(tree == NULL || find_avl(tree, value) == NULL);
}

static char *capture_relation_output(ManyToManyRelation *relation, int id, int search_products) {
    int saved_stdout = dup(fileno(stdout));
    FILE *temp = tmpfile();

    assert(saved_stdout != -1);
    assert(temp != NULL);
    assert(dup2(fileno(temp), fileno(stdout)) != -1);

    if (search_products) {
        print_products_for_customer(relation, id);
    } else {
        print_customers_for_product(relation, id);
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

static void test_create_relation_starts_empty(void) {
    ManyToManyRelation relation = create_relation(11, 11);

    assert(find_products_for_customer(&relation, 150) == NULL);
    assert(find_customers_for_product(&relation, 156) == NULL);

    destroy_relation(&relation);
}

static void test_insert_relationship_builds_both_directions(void) {
    ManyToManyRelation relation = create_relation(11, 11);

    insert_relationship(&relation, 150, 156);

    assert_tree_contains(find_products_for_customer(&relation, 150), 156);
    assert_tree_contains(find_customers_for_product(&relation, 156), 150);
    assert_tree_missing(find_products_for_customer(&relation, 999), 156);

    destroy_relation(&relation);
}

static void test_many_to_many_relationships_are_mirrored(void) {
    ManyToManyRelation relation = create_relation(11, 11);

    insert_relationship(&relation, 150, 156);
    insert_relationship(&relation, 150, 200);
    insert_relationship(&relation, 300, 156);
    insert_relationship(&relation, 300, 200);

    assert_tree_contains(find_products_for_customer(&relation, 150), 156);
    assert_tree_contains(find_products_for_customer(&relation, 150), 200);
    assert_tree_contains(find_products_for_customer(&relation, 300), 156);
    assert_tree_contains(find_products_for_customer(&relation, 300), 200);
    assert_tree_contains(find_customers_for_product(&relation, 156), 150);
    assert_tree_contains(find_customers_for_product(&relation, 156), 300);
    assert_tree_contains(find_customers_for_product(&relation, 200), 150);
    assert_tree_contains(find_customers_for_product(&relation, 200), 300);

    destroy_relation(&relation);
}

static void test_duplicate_relationship_does_not_duplicate_avl_entry(void) {
    ManyToManyRelation relation = create_relation(11, 11);
    AVL *products;
    AVL *customers;

    insert_relationship(&relation, 150, 156);
    insert_relationship(&relation, 150, 156);

    products = find_products_for_customer(&relation, 150);
    customers = find_customers_for_product(&relation, 156);

    assert(products != NULL);
    assert(customers != NULL);
    assert(products->root != NULL);
    assert(customers->root != NULL);
    assert(products->root->data_item == 156);
    assert(products->root->left == NULL);
    assert(products->root->right == NULL);
    assert(customers->root->data_item == 150);
    assert(customers->root->left == NULL);
    assert(customers->root->right == NULL);

    destroy_relation(&relation);
}

static void test_load_relationships_from_file_builds_both_indexes(void) {
    const char *path = "/tmp/prototype1_test_data.txt";
    FILE *file;
    ManyToManyRelation relation = create_relation(11, 11);
    int count;

    file = fopen(path, "w");
    assert(file != NULL);

    fprintf(file, "150 156\n");
    fprintf(file, "150 200\n");
    fprintf(file, "300 156\n");
    fprintf(file, "400 500\n");
    fclose(file);

    count = load_relationships_from_file(&relation, path);

    assert(count == 4);
    assert_tree_contains(find_products_for_customer(&relation, 150), 156);
    assert_tree_contains(find_products_for_customer(&relation, 150), 200);
    assert_tree_contains(find_products_for_customer(&relation, 300), 156);
    assert_tree_contains(find_customers_for_product(&relation, 156), 150);
    assert_tree_contains(find_customers_for_product(&relation, 156), 300);
    assert_tree_contains(find_customers_for_product(&relation, 500), 400);

    destroy_relation(&relation);
    unlink(path);
}

static void test_load_relationships_from_missing_file_returns_error(void) {
    ManyToManyRelation relation = create_relation(11, 11);

    assert(load_relationships_from_file(&relation, "/tmp/does_not_exist_relationships.txt") == -1);

    destroy_relation(&relation);
}

static void test_print_helpers_show_matching_ids_or_none(void) {
    ManyToManyRelation relation = create_relation(11, 11);
    char *products_output;
    char *customers_output;
    char *missing_output;

    insert_relationship(&relation, 150, 156);
    insert_relationship(&relation, 150, 200);
    insert_relationship(&relation, 300, 156);

    products_output = capture_relation_output(&relation, 150, 1);
    customers_output = capture_relation_output(&relation, 156, 0);
    missing_output = capture_relation_output(&relation, 999, 1);

    assert(strcmp(products_output, "Products for customer 150: 156 200\n") == 0);
    assert(strcmp(customers_output, "Customers for product 156: 150 300\n") == 0);
    assert(strcmp(missing_output, "Products for customer 999: none\n") == 0);

    free(products_output);
    free(customers_output);
    free(missing_output);
    destroy_relation(&relation);
}

void run_relation_for_avl_tests(void) {
    test_create_relation_starts_empty();
    test_insert_relationship_builds_both_directions();
    test_many_to_many_relationships_are_mirrored();
    test_duplicate_relationship_does_not_duplicate_avl_entry();
    test_load_relationships_from_file_builds_both_indexes();
    test_load_relationships_from_missing_file_returns_error();
    test_print_helpers_show_matching_ids_or_none();
}
