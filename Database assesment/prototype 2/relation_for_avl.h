#pragma once

#include "avl.h"

typedef struct relationEntry {
    int key;
    AVL values;
    struct relationEntry *next;
} RelationEntry;

typedef struct relationIndex {
    int size;
    RelationEntry **buckets;
} RelationIndex;

typedef struct manyToManyRelation {
    RelationIndex customers_to_products;
    RelationIndex products_to_customers;
} ManyToManyRelation;

ManyToManyRelation create_relation(int customer_bucket_count, int product_bucket_count);
void destroy_relation(ManyToManyRelation *self);
void insert_relationship(ManyToManyRelation *self, int customer_id, int product_id);
int load_relationships_from_file(ManyToManyRelation *self, const char *filename);
AVL *find_products_for_customer(ManyToManyRelation *self, int customer_id);
AVL *find_customers_for_product(ManyToManyRelation *self, int product_id);
void print_products_for_customer(ManyToManyRelation *self, int customer_id);
void print_customers_for_product(ManyToManyRelation *self, int product_id);
long long count_relation_read_values(ManyToManyRelation *self);
