#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include "prototype1.h"

static void assert_tree_contains(AVL *tree, int value) {
    assert(tree != NULL);
    assert(find_avl(tree, value) != NULL);
}

static void assert_tree_missing(AVL *tree, int value) {
    assert(tree == NULL || find_avl(tree, value) == NULL);
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

int main(void) {
    test_create_relation_starts_empty();
    test_insert_relationship_builds_both_directions();
    test_many_to_many_relationships_are_mirrored();
    test_duplicate_relationship_does_not_duplicate_avl_entry();
    test_load_relationships_from_file_builds_both_indexes();
    test_load_relationships_from_missing_file_returns_error();

    printf("All prototype1 tests passed.\n");
    return 0;
}
