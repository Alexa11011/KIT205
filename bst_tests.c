#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "bst.h"

static void insert_values(BST *tree, const int *values, size_t count) {
    for (size_t i = 0; i < count; i++) {
        insert_bst(tree, values[i]);
    }
}

static char *capture_in_order_output(BST *tree) {
    int saved_stdout = dup(fileno(stdout));
    FILE *temp = tmpfile();

    assert(saved_stdout != -1);
    assert(temp != NULL);
    assert(dup2(fileno(temp), fileno(stdout)) != -1);

    print_in_order_bst(tree);
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

static void test_new_bst_starts_empty(void) {
    BST tree = new_bst();

    assert(tree.root == NULL);
    assert(find_bst(&tree, 42) == NULL);
    assert(bst_height(tree.root) == -1);
}

static void test_insert_and_find_build_expected_structure(void) {
    BST tree = new_bst();
    int values[] = {5, 3, 7, 2, 4, 6, 8};

    insert_values(&tree, values, sizeof values / sizeof values[0]);

    assert(tree.root != NULL);
    assert(tree.root->data_item == 5);
    assert(tree.root->left->data_item == 3);
    assert(tree.root->right->data_item == 7);
    assert(find_bst(&tree, 2)->data_item == 2);
    assert(find_bst(&tree, 4)->data_item == 4);
    assert(find_bst(&tree, 6)->data_item == 6);
    assert(find_bst(&tree, 8)->data_item == 8);
    assert(find_bst(&tree, 99) == NULL);

    destroy_bst(&tree);
}

static void test_insert_ignores_duplicate_values(void) {
    BST tree = new_bst();

    insert_bst(&tree, 5);
    insert_bst(&tree, 3);
    insert_bst(&tree, 7);
    insert_bst(&tree, 3);
    insert_bst(&tree, 7);
    insert_bst(&tree, 5);

    assert(tree.root->data_item == 5);
    assert(tree.root->left->data_item == 3);
    assert(tree.root->right->data_item == 7);
    assert(tree.root->left->left == NULL);
    assert(tree.root->left->right == NULL);
    assert(tree.root->right->left == NULL);
    assert(tree.root->right->right == NULL);
    assert(bst_height(tree.root) == 1);

    destroy_bst(&tree);
}

static void test_delete_leaf_node_removes_value(void) {
    BST tree = new_bst();
    int values[] = {5, 3, 7, 2};

    insert_values(&tree, values, sizeof values / sizeof values[0]);
    delete_bst(&tree, 2);

    assert(find_bst(&tree, 2) == NULL);
    assert(tree.root->left->left == NULL);

    destroy_bst(&tree);
}

static void test_delete_node_with_one_child_promotes_child(void) {
    BST tree = new_bst();
    int values[] = {5, 3, 2, 7};

    insert_values(&tree, values, sizeof values / sizeof values[0]);
    delete_bst(&tree, 3);

    assert(find_bst(&tree, 3) == NULL);
    assert(tree.root->left != NULL);
    assert(tree.root->left->data_item == 2);
    assert(tree.root->left->left == NULL);
    assert(tree.root->left->right == NULL);

    destroy_bst(&tree);
}

static void test_delete_node_with_two_children_uses_successor(void) {
    BST tree = new_bst();
    int values[] = {5, 3, 7, 6, 8};

    insert_values(&tree, values, sizeof values / sizeof values[0]);
    delete_bst(&tree, 7);

    assert(find_bst(&tree, 7) == NULL);
    assert(tree.root->right != NULL);
    assert(tree.root->right->data_item == 8);
    assert(tree.root->right->left->data_item == 6);
    assert(tree.root->right->right == NULL);

    destroy_bst(&tree);
}

static void test_delete_root_with_two_children_updates_root_value(void) {
    BST tree = new_bst();
    int values[] = {5, 3, 7, 6};

    insert_values(&tree, values, sizeof values / sizeof values[0]);
    delete_bst(&tree, 5);

    assert(tree.root != NULL);
    assert(tree.root->data_item == 6);
    assert(find_bst(&tree, 5) == NULL);
    assert(tree.root->left->data_item == 3);
    assert(tree.root->right->data_item == 7);

    destroy_bst(&tree);
}

static void test_delete_missing_value_leaves_tree_unchanged(void) {
    BST tree = new_bst();
    int values[] = {5, 3, 7};

    insert_values(&tree, values, sizeof values / sizeof values[0]);
    delete_bst(&tree, 99);

    assert(tree.root->data_item == 5);
    assert(tree.root->left->data_item == 3);
    assert(tree.root->right->data_item == 7);

    destroy_bst(&tree);
}

static void test_print_in_order_matches_expected_format(void) {
    BST tree = new_bst();
    int values[] = {5, 3, 7};

    insert_values(&tree, values, sizeof values / sizeof values[0]);

    char *output = capture_in_order_output(&tree);
    assert(strcmp(output, "((_ 3 _) 5 (_ 7 _))") == 0);

    free(output);
    destroy_bst(&tree);
}

static void test_destroy_bst_clears_root(void) {
    BST tree = new_bst();
    int values[] = {5, 3, 7};

    insert_values(&tree, values, sizeof values / sizeof values[0]);
    destroy_bst(&tree);

    assert(tree.root == NULL);
    assert(find_bst(&tree, 5) == NULL);
}

static void test_bst_height_reports_longest_path_in_edges(void) {
    BST empty_tree = new_bst();
    BST single_node_tree = new_bst();
    BST shaped_tree = new_bst();
    int values[] = {5, 3, 7, 2, 1};

    insert_bst(&single_node_tree, 10);
    insert_values(&shaped_tree, values, sizeof values / sizeof values[0]);

    assert(bst_height(empty_tree.root) == -1);
    assert(bst_height(single_node_tree.root) == 0);
    assert(bst_height(shaped_tree.root) == 3);

    destroy_bst(&single_node_tree);
    destroy_bst(&shaped_tree);
}

int main(void) {
    test_new_bst_starts_empty();
    test_insert_and_find_build_expected_structure();
    test_insert_ignores_duplicate_values();
    test_delete_leaf_node_removes_value();
    test_delete_node_with_one_child_promotes_child();
    test_delete_node_with_two_children_uses_successor();
    test_delete_root_with_two_children_updates_root_value();
    test_delete_missing_value_leaves_tree_unchanged();
    test_print_in_order_matches_expected_format();
    test_destroy_bst_clears_root();
    test_bst_height_reports_longest_path_in_edges();

    printf("All BST tests passed.\n");
    return 0;
}
