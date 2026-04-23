#include <stdio.h>
#include <stdlib.h>

#include "prototype1.h"

static int hash_int_key(int key, int size) {
    unsigned int value = (unsigned int) key;
    return (int) (value % (unsigned int) size);
}

static RelationIndex create_relation_index(int bucket_count) {
    RelationIndex index;

    index.size = bucket_count;
    index.buckets = calloc((size_t) bucket_count, sizeof(RelationEntry *));

    if (index.buckets == NULL) {
        fprintf(stderr, "Error allocating relation index.\n");
        exit(EXIT_FAILURE);
    }

    return index;
}

static void destroy_relation_index(RelationIndex *self) {
    for (int i = 0; i < self->size; i++) {
        RelationEntry *current = self->buckets[i];

        while (current != NULL) {
            RelationEntry *to_free = current;

            current = current->next;
            destroy_avl(&(to_free->values));
            free(to_free);
        }
    }

    free(self->buckets);
    self->buckets = NULL;
    self->size = 0;
}

static RelationEntry *find_relation_entry(RelationIndex *self, int key) {
    RelationEntry *current = self->buckets[hash_int_key(key, self->size)];

    while (current != NULL) {
        if (current->key == key) {
            return current;
        }

        current = current->next;
    }

    return NULL;
}

static RelationEntry *get_or_create_relation_entry(RelationIndex *self, int key) {
    int bucket_index = hash_int_key(key, self->size);
    RelationEntry *current = self->buckets[bucket_index];

    while (current != NULL) {
        if (current->key == key) {
            return current;
        }

        current = current->next;
    }

    current = malloc(sizeof *current);

    if (current == NULL) {
        fprintf(stderr, "Error allocating relation entry.\n");
        exit(EXIT_FAILURE);
    }

    current->key = key;
    current->values = new_avl();
    current->next = self->buckets[bucket_index];
    self->buckets[bucket_index] = current;

    return current;
}

ManyToManyRelation create_relation(int customer_bucket_count, int product_bucket_count) {
    ManyToManyRelation relation;

    relation.customers_to_products = create_relation_index(customer_bucket_count);
    relation.products_to_customers = create_relation_index(product_bucket_count);

    return relation;
}

void destroy_relation(ManyToManyRelation *self) {
    destroy_relation_index(&(self->customers_to_products));
    destroy_relation_index(&(self->products_to_customers));
}

void insert_relationship(ManyToManyRelation *self, int customer_id, int product_id) {
    RelationEntry *customer_entry = get_or_create_relation_entry(&(self->customers_to_products), customer_id);
    RelationEntry *product_entry = get_or_create_relation_entry(&(self->products_to_customers), product_id);

    insert_avl(&(customer_entry->values), product_id);
    insert_avl(&(product_entry->values), customer_id);
}

int load_relationships_from_file(ManyToManyRelation *self, const char *filename) {
    FILE *file = fopen(filename, "r");
    int customer_id;
    int product_id;
    int inserted_count = 0;

    if (file == NULL) {
        return -1;
    }

    while (fscanf(file, "%d %d", &customer_id, &product_id) == 2) {
        insert_relationship(self, customer_id, product_id);
        inserted_count++;
    }

    fclose(file);
    return inserted_count;
}

AVL *find_products_for_customer(ManyToManyRelation *self, int customer_id) {
    RelationEntry *entry = find_relation_entry(&(self->customers_to_products), customer_id);

    if (entry == NULL) {
        return NULL;
    }

    return &(entry->values);
}

AVL *find_customers_for_product(ManyToManyRelation *self, int product_id) {
    RelationEntry *entry = find_relation_entry(&(self->products_to_customers), product_id);

    if (entry == NULL) {
        return NULL;
    }

    return &(entry->values);
}
