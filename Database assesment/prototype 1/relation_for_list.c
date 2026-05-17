#include <stdio.h>
#include <stdlib.h>

#include "relation_for_list.h"

static int hash_int_key(int key, int size) {
    unsigned int value = (unsigned int) key;
    return (int) (value % (unsigned int) size);
}

static ListRelationIndex create_list_relation_index(int bucket_count) {
    ListRelationIndex index;

    index.size = bucket_count;
    index.buckets = calloc((size_t) bucket_count, sizeof(ListRelationEntry *));

    if (index.buckets == NULL) {
        fprintf(stderr, "Error allocating list relation index.\n");
        exit(EXIT_FAILURE);
    }

    return index;
}

static void destroy_list_relation_index(ListRelationIndex *self) {
    for (int i = 0; i < self->size; i++) {
        ListRelationEntry *current = self->buckets[i];

        while (current != NULL) {
            ListRelationEntry *to_free = current;

            current = current->next;
            destroy_list(&(to_free->values));
            free(to_free);
        }
    }

    free(self->buckets);
    self->buckets = NULL;
    self->size = 0;
}

static ListRelationEntry *find_list_relation_entry(ListRelationIndex *self, int key) {
    ListRelationEntry *current = self->buckets[hash_int_key(key, self->size)];

    while (current != NULL) {
        if (current->key == key) {
            return current;
        }

        current = current->next;
    }

    return NULL;
}

static ListRelationEntry *get_or_create_list_relation_entry(ListRelationIndex *self, int key) {
    int bucket_index = hash_int_key(key, self->size);
    ListRelationEntry *current = self->buckets[bucket_index];

    while (current != NULL) {
        if (current->key == key) {
            return current;
        }

        current = current->next;
    }

    current = malloc(sizeof *current);

    if (current == NULL) {
        fprintf(stderr, "Error allocating list relation entry.\n");
        exit(EXIT_FAILURE);
    }

    current->key = key;
    current->values = new_list();
    current->next = self->buckets[bucket_index];
    self->buckets[bucket_index] = current;

    return current;
}

static int compare_ints(const void *left, const void *right) {
    int a = *(const int *) left;
    int b = *(const int *) right;

    return (a > b) - (a < b);
}

static int list_length(ListNodePtr current) {
    int count = 0;

    while (current != NULL) {
        count++;
        current = current->next;
    }

    return count;
}

static int *copy_sorted_values(ListNodePtr current, int *count) {
    int *values;
    int i = 0;

    *count = list_length(current);

    if (*count == 0) {
        return NULL;
    }

    values = malloc(sizeof(int) * (size_t) *count);

    if (values == NULL) {
        fprintf(stderr, "Error allocating sorted list values.\n");
        exit(EXIT_FAILURE);
    }

    while (current != NULL) {
        values[i] = current->data;
        i++;
        current = current->next;
    }

    qsort(values, (size_t) *count, sizeof(int), compare_ints);
    return values;
}

static long long count_sorted_list_values(List *list) {
    int count;
    int *values = copy_sorted_values(list->head, &count);

    free(values);
    return count;
}

static long long count_sorted_index_values(ListRelationIndex *index) {
    long long count = 0;

    for (int i = 0; i < index->size; i++) {
        ListRelationEntry *current = index->buckets[i];

        while (current != NULL) {
            count += count_sorted_list_values(&(current->values));
            current = current->next;
        }
    }

    return count;
}

static int list_contains(List *list, int value) {
    ListNodePtr current = list->head;

    while (current != NULL) {
        if (current->data == value) {
            return 1;
        }

        current = current->next;
    }

    return 0;
}

static void print_sorted_list_lookup(const char *prefix, int key, List *values) {
    int count;
    int *sorted_values;

    printf("%s %d: ", prefix, key);

    if (values == NULL || values->head == NULL) {
        printf("none\n");
        return;
    }

    sorted_values = copy_sorted_values(values->head, &count);

    for (int i = 0; i < count; i++) {
        if (i > 0) {
            printf(" ");
        }

        printf("%d", sorted_values[i]);
    }

    printf("\n");
    free(sorted_values);
}

ListManyToManyRelation create_list_relation(int customer_bucket_count, int product_bucket_count) {
    ListManyToManyRelation relation;

    relation.customers_to_products = create_list_relation_index(customer_bucket_count);
    relation.products_to_customers = create_list_relation_index(product_bucket_count);

    return relation;
}

void destroy_list_relation(ListManyToManyRelation *self) {
    destroy_list_relation_index(&(self->customers_to_products));
    destroy_list_relation_index(&(self->products_to_customers));
}

void insert_list_relationship(ListManyToManyRelation *self, int customer_id, int product_id) {
    ListRelationEntry *customer_entry = get_or_create_list_relation_entry(&(self->customers_to_products), customer_id);
    ListRelationEntry *product_entry = get_or_create_list_relation_entry(&(self->products_to_customers), product_id);

    if (list_contains(&(customer_entry->values), product_id)) {
        return;
    }

    insert_at_front(&(customer_entry->values), product_id);
    insert_at_front(&(product_entry->values), customer_id);
}

int load_list_relationships_from_file(ListManyToManyRelation *self, const char *filename) {
    FILE *file = fopen(filename, "r");
    int customer_id;
    int product_id;
    int inserted_count = 0;

    if (file == NULL) {
        return -1;
    }

    while (fscanf(file, "%d %d", &customer_id, &product_id) == 2) {
        insert_list_relationship(self, customer_id, product_id);
        inserted_count++;
    }

    fclose(file);
    return inserted_count;
}

void print_list_products_for_customer(ListManyToManyRelation *self, int customer_id) {
    ListRelationEntry *entry = find_list_relation_entry(&(self->customers_to_products), customer_id);

    print_sorted_list_lookup("Products for customer", customer_id, entry == NULL ? NULL : &(entry->values));
}

void print_list_customers_for_product(ListManyToManyRelation *self, int product_id) {
    ListRelationEntry *entry = find_list_relation_entry(&(self->products_to_customers), product_id);

    print_sorted_list_lookup("Customers for product", product_id, entry == NULL ? NULL : &(entry->values));
}

long long count_list_relation_read_values(ListManyToManyRelation *self) {
    return count_sorted_index_values(&(self->customers_to_products))
        + count_sorted_index_values(&(self->products_to_customers));
}
