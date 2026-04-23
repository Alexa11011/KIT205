#include <stdio.h>
#include <stdlib.h>

#include "avl.h"

static int max_int(int a, int b) {
    return (a > b) ? a : b;
}

int avl_height(AVLNodePtr self) {
    if (self == NULL) {
        return -1;
    }

    return self->height;
}

static void update_height(AVLNodePtr self) {
    if (self != NULL) {
        self->height = 1 + max_int(avl_height(self->left), avl_height(self->right));
    }
}

static int balance_factor(AVLNodePtr self) {
    if (self == NULL) {
        return 0;
    }

    return avl_height(self->left) - avl_height(self->right);
}

static AVLNodePtr rotate_right(AVLNodePtr self) {
    AVLNodePtr new_root = self->left;
    AVLNodePtr moved_subtree = new_root->right;

    new_root->right = self;
    self->left = moved_subtree;

    update_height(self);
    update_height(new_root);

    return new_root;
}

static AVLNodePtr rotate_left(AVLNodePtr self) {
    AVLNodePtr new_root = self->right;
    AVLNodePtr moved_subtree = new_root->left;

    new_root->left = self;
    self->right = moved_subtree;

    update_height(self);
    update_height(new_root);

    return new_root;
}

static AVLNodePtr rebalance(AVLNodePtr self) {
    int balance;

    if (self == NULL) {
        return NULL;
    }

    update_height(self);
    balance = balance_factor(self);

    if (balance > 1) {
        if (balance_factor(self->left) < 0) {
            self->left = rotate_left(self->left);
        }
        return rotate_right(self);
    }

    if (balance < -1) {
        if (balance_factor(self->right) > 0) {
            self->right = rotate_right(self->right);
        }
        return rotate_left(self);
    }

    return self;
}

AVL new_avl(void) {
    AVL temp;
    temp.root = NULL;
    return temp;
}

static AVLNodePtr create_avl_node(int n) {
    AVLNodePtr node = malloc(sizeof *node);

    if (node == NULL) {
        fprintf(stderr, "Error allocating AVL node.\n");
        exit(EXIT_FAILURE);
    }

    node->data_item = n;
    node->height = 0;
    node->left = NULL;
    node->right = NULL;

    return node;
}

static AVLNodePtr find_avl_node(AVLNodePtr self, int n) {
    if (self == NULL || n == self->data_item) {
        return self;
    } else if (n < self->data_item) {
        return find_avl_node(self->left, n);
    } else {
        return find_avl_node(self->right, n);
    }
}

AVLNodePtr find_avl(AVL *self, int n) {
    return find_avl_node(self->root, n);
}

static AVLNodePtr insert_avl_node(AVLNodePtr self, int n) {
    if (self == NULL) {
        return create_avl_node(n);
    } else if (n < self->data_item) {
        self->left = insert_avl_node(self->left, n);
    } else if (n > self->data_item) {
        self->right = insert_avl_node(self->right, n);
    } else {
        return self;
    }

    return rebalance(self);
}

void insert_avl(AVL *self, int n) {
    self->root = insert_avl_node(self->root, n);
}

static AVLNodePtr min_avl_node(AVLNodePtr self) {
    AVLNodePtr current = self;

    while (current->left != NULL) {
        current = current->left;
    }

    return current;
}

static AVLNodePtr delete_avl_node(AVLNodePtr self, int n) {
    if (self == NULL) {
        return NULL;
    }

    if (n < self->data_item) {
        self->left = delete_avl_node(self->left, n);
    } else if (n > self->data_item) {
        self->right = delete_avl_node(self->right, n);
    } else {
        if (self->left != NULL && self->right != NULL) {
            AVLNodePtr successor = min_avl_node(self->right);
            self->data_item = successor->data_item;
            self->right = delete_avl_node(self->right, successor->data_item);
        } else {
            AVLNodePtr child = (self->left != NULL) ? self->left : self->right;
            free(self);
            return child;
        }
    }

    return rebalance(self);
}

void delete_avl(AVL *self, int n) {
    self->root = delete_avl_node(self->root, n);
}

static void print_in_order_avl_node(AVLNodePtr self) {
    if (self == NULL) {
        printf("_");
    } else {
        printf("(");
        print_in_order_avl_node(self->left);
        printf(" %d ", self->data_item);
        print_in_order_avl_node(self->right);
        printf(")");
    }
}

void print_in_order_avl(AVL *self) {
    print_in_order_avl_node(self->root);
}

static void print_pre_order_avl_node(AVLNodePtr self) {
    if (self == NULL) {
        printf("_");
    } else {
        printf("(");
        printf(" %d ", self->data_item);
        print_pre_order_avl_node(self->left);
        print_pre_order_avl_node(self->right);
        printf(")");
    }
}

void print_pre_order_avl(AVL *self) {
    print_pre_order_avl_node(self->root);
}

static void print_post_order_avl_node(AVLNodePtr self) {
    if (self == NULL) {
        printf("_");
    } else {
        printf("(");
        print_post_order_avl_node(self->left);
        print_post_order_avl_node(self->right);
        printf(" %d ", self->data_item);
        printf(")");
    }
}

void print_post_order_avl(AVL *self) {
    print_post_order_avl_node(self->root);
}

static void destroy_avl_node(AVLNodePtr self) {
    if (self != NULL) {
        destroy_avl_node(self->left);
        destroy_avl_node(self->right);
        free(self);
    }
}

void destroy_avl(AVL *self) {
    destroy_avl_node(self->root);
    self->root = NULL;
}
