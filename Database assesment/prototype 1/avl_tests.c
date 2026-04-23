#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "avl.h"

static void insert_values(AVL *tree, const int *values, size_t count) {
    for (size_t i = 0; i < count; i++) {
        insert_avl(tree, values[i]);
    }
}

static int max_int(int a, int b) {
    return (a > b) ? a : b;
}

static int verify_avl_subtree(AVLNodePtr node) {
    if (node == NULL) {
        return -1;
    }

    int left_height = verify_avl_subtree(node->left);
    int right_height = verify_avl_subtree(node->right);
    int balance = left_height - right_height;

    assert(balance >= -1 && balance <= 1);
    assert(node->height == 1 + max_int(left_height, right_height));

    if (node->left != NULL) {
        assert(node->left->data_item < node->data_item);
    }

    if (node->right != NULL) {
        assert(node->right->data_item > node->data_item);
    }

    return node->height;
}

static void assert_valid_avl(AVL *tree) {
    assert(verify_avl_subtree(tree->root) == avl_height(tree->root));
}

static char *capture_in_order_output(AVL *tree) {
    int saved_stdout = dup(fileno(stdout));
    FILE *temp = tmpfile();

    assert(saved_stdout != -1);
    assert(temp != NULL);
    assert(dup2(fileno(temp), fileno(stdout)) != -1);

    print_in_order_avl(tree);
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

static void test_new_avl_starts_empty(void) {
    AVL tree = new_avl();

    assert(tree.root == NULL);
    assert(find_avl(&tree, 42) == NULL);
    assert(avl_height(tree.root) == -1);
}

static void test_insert_performs_left_left_rotation(void) {
    AVL tree = new_avl();
    int values[] = {30, 20, 10};

    insert_values(&tree, values, sizeof values / sizeof values[0]);

    assert(tree.root != NULL);
    assert(tree.root->data_item == 20);
    assert(tree.root->left->data_item == 10);
    assert(tree.root->right->data_item == 30);
    assert(tree.root->height == 1);
    assert_valid_avl(&tree);

    destroy_avl(&tree);
}

static void test_insert_performs_right_right_rotation(void) {
    AVL tree = new_avl();
    int values[] = {10, 20, 30};

    insert_values(&tree, values, sizeof values / sizeof values[0]);

    assert(tree.root != NULL);
    assert(tree.root->data_item == 20);
    assert(tree.root->left->data_item == 10);
    assert(tree.root->right->data_item == 30);
    assert(tree.root->height == 1);
    assert_valid_avl(&tree);

    destroy_avl(&tree);
}

static void test_insert_performs_left_right_rotation(void) {
    AVL tree = new_avl();
    int values[] = {30, 10, 20};

    insert_values(&tree, values, sizeof values / sizeof values[0]);

    assert(tree.root != NULL);
    assert(tree.root->data_item == 20);
    assert(tree.root->left->data_item == 10);
    assert(tree.root->right->data_item == 30);
    assert(tree.root->height == 1);
    assert_valid_avl(&tree);

    destroy_avl(&tree);
}

static void test_insert_performs_right_left_rotation(void) {
    AVL tree = new_avl();
    int values[] = {10, 30, 20};

    insert_values(&tree, values, sizeof values / sizeof values[0]);

    assert(tree.root != NULL);
    assert(tree.root->data_item == 20);
    assert(tree.root->left->data_item == 10);
    assert(tree.root->right->data_item == 30);
    assert(tree.root->height == 1);
    assert_valid_avl(&tree);

    destroy_avl(&tree);
}

static void test_insert_and_find_build_balanced_tree(void) {
    AVL tree = new_avl();
    int values[] = {50, 30, 70, 20, 40, 60, 80, 10, 35, 65};

    insert_values(&tree, values, sizeof values / sizeof values[0]);

    assert(find_avl(&tree, 10)->data_item == 10);
    assert(find_avl(&tree, 35)->data_item == 35);
    assert(find_avl(&tree, 65)->data_item == 65);
    assert(find_avl(&tree, 999) == NULL);
    assert_valid_avl(&tree);

    destroy_avl(&tree);
}

static void test_insert_ignores_duplicate_values(void) {
    AVL tree = new_avl();
    int values[] = {20, 10, 30, 10, 20, 30};

    insert_values(&tree, values, sizeof values / sizeof values[0]);

    assert(tree.root != NULL);
    assert(tree.root->data_item == 20);
    assert(tree.root->left->data_item == 10);
    assert(tree.root->right->data_item == 30);
    assert(tree.root->left->left == NULL);
    assert(tree.root->left->right == NULL);
    assert(tree.root->right->left == NULL);
    assert(tree.root->right->right == NULL);
    assert_valid_avl(&tree);

    destroy_avl(&tree);
}

static void test_delete_leaf_node_keeps_tree_balanced(void) {
    AVL tree = new_avl();
    int values[] = {20, 10, 30, 5, 15};

    insert_values(&tree, values, sizeof values / sizeof values[0]);
    delete_avl(&tree, 5);

    assert(find_avl(&tree, 5) == NULL);
    assert_valid_avl(&tree);

    destroy_avl(&tree);
}

static void test_delete_node_with_one_child_promotes_child(void) {
    AVL tree = new_avl();
    int values[] = {20, 10, 30, 5};

    insert_values(&tree, values, sizeof values / sizeof values[0]);
    delete_avl(&tree, 10);

    assert(find_avl(&tree, 10) == NULL);
    assert(tree.root->left != NULL);
    assert(tree.root->left->data_item == 5);
    assert_valid_avl(&tree);

    destroy_avl(&tree);
}

static void test_delete_node_with_two_children_uses_successor_and_rebalances(void) {
    AVL tree = new_avl();
    int values[] = {40, 20, 60, 10, 30, 50, 70, 25, 35};

    insert_values(&tree, values, sizeof values / sizeof values[0]);
    delete_avl(&tree, 20);

    assert(find_avl(&tree, 20) == NULL);
    assert(find_avl(&tree, 25) != NULL);
    assert_valid_avl(&tree);

    destroy_avl(&tree);
}

static void test_delete_root_rebalances_tree(void) {
    AVL tree = new_avl();
    int values[] = {9, 5, 10, 0, 6, 11, -1, 1, 2};

    insert_values(&tree, values, sizeof values / sizeof values[0]);
    delete_avl(&tree, 10);

    assert(find_avl(&tree, 10) == NULL);
    assert(tree.root != NULL);
    assert(tree.root->data_item == 1);
    assert_valid_avl(&tree);

    destroy_avl(&tree);
}

static void test_delete_missing_value_leaves_tree_unchanged(void) {
    AVL tree = new_avl();
    int values[] = {20, 10, 30};

    insert_values(&tree, values, sizeof values / sizeof values[0]);
    delete_avl(&tree, 99);

    assert(tree.root != NULL);
    assert(tree.root->data_item == 20);
    assert(tree.root->left->data_item == 10);
    assert(tree.root->right->data_item == 30);
    assert_valid_avl(&tree);

    destroy_avl(&tree);
}

static void test_print_in_order_matches_expected_format(void) {
    AVL tree = new_avl();
    int values[] = {30, 20, 40, 10, 25};

    insert_values(&tree, values, sizeof values / sizeof values[0]);

    char *output = capture_in_order_output(&tree);
    assert(strcmp(output, "(((_ 10 _) 20 (_ 25 _)) 30 (_ 40 _))") == 0);

    free(output);
    destroy_avl(&tree);
}

static void test_destroy_avl_clears_root(void) {
    AVL tree = new_avl();
    int values[] = {20, 10, 30};

    insert_values(&tree, values, sizeof values / sizeof values[0]);
    destroy_avl(&tree);

    assert(tree.root == NULL);
    assert(find_avl(&tree, 20) == NULL);
}

static void test_avl_height_reports_longest_path_in_edges(void) {
    AVL empty_tree = new_avl();
    AVL single_node_tree = new_avl();
    AVL shaped_tree = new_avl();
    int values[] = {30, 20, 40, 10, 25, 5};

    insert_avl(&single_node_tree, 10);
    insert_values(&shaped_tree, values, sizeof values / sizeof values[0]);

    assert(avl_height(empty_tree.root) == -1);
    assert(avl_height(single_node_tree.root) == 0);
    assert(avl_height(shaped_tree.root) == 2);
    assert_valid_avl(&single_node_tree);
    assert_valid_avl(&shaped_tree);

    destroy_avl(&single_node_tree);
    destroy_avl(&shaped_tree);
}

int main(void) {
    test_new_avl_starts_empty();
    test_insert_performs_left_left_rotation();
    test_insert_performs_right_right_rotation();
    test_insert_performs_left_right_rotation();
    test_insert_performs_right_left_rotation();
    test_insert_and_find_build_balanced_tree();
    test_insert_ignores_duplicate_values();
    test_delete_leaf_node_keeps_tree_balanced();
    test_delete_node_with_one_child_promotes_child();
    test_delete_node_with_two_children_uses_successor_and_rebalances();
    test_delete_root_rebalances_tree();
    test_delete_missing_value_leaves_tree_unchanged();
    test_print_in_order_matches_expected_format();
    test_destroy_avl_clears_root();
    test_avl_height_reports_longest_path_in_edges();

    printf("All AVL tests passed.\n");
    return 0;
}
