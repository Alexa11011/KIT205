#pragma once

typedef struct avlNode {
    int data_item;
    int height;
    struct avlNode *left;
    struct avlNode *right;
} *AVLNodePtr;

typedef struct avl {
    AVLNodePtr root;
} AVL;

AVL new_avl(void);
AVLNodePtr find_avl(AVL *self, int n);
void insert_avl(AVL *self, int n);
void delete_avl(AVL *self, int n);
void print_in_order_avl(AVL *self);
void print_pre_order_avl(AVL *self);
void print_post_order_avl(AVL *self);
void destroy_avl(AVL *self);
int avl_height(AVLNodePtr self);
