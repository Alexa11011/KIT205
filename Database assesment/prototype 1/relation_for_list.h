#pragma once

#include "list.h"

typedef struct listRelationEntry {
    int key;
    List values;
    struct listRelationEntry *next;
} ListRelationEntry;

typedef struct listRelationIndex {
    int size;
    ListRelationEntry **buckets;
} ListRelationIndex;

typedef struct listManyToManyRelation {
    ListRelationIndex customers_to_products;
    ListRelationIndex products_to_customers;
} ListManyToManyRelation;

ListManyToManyRelation create_list_relation(int customer_bucket_count, int product_bucket_count);
void destroy_list_relation(ListManyToManyRelation *self);
void insert_list_relationship(ListManyToManyRelation *self, int customer_id, int product_id);
int load_list_relationships_from_file(ListManyToManyRelation *self, const char *filename);
void print_list_products_for_customer(ListManyToManyRelation *self, int customer_id);
void print_list_customers_for_product(ListManyToManyRelation *self, int product_id);
long long count_list_relation_read_values(ListManyToManyRelation *self);
