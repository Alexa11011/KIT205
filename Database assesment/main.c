#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "prototype1.h"

static int load_default_relation(ManyToManyRelation *relation) {
    int count = load_relationships_from_file(relation, "Database assesment/data.txt");

    if (count >= 0) {
        return count;
    }

    return load_relationships_from_file(relation, "data.txt");
}

int main(void) {
    ManyToManyRelation relation = create_relation(1009, 509);
    int relationship_count = load_default_relation(&relation);
    int choice;
    int id;

    if (relationship_count < 0) {
        fprintf(stderr, "Could not load relationship data from Database assesment/data.txt or data.txt\n");
        destroy_relation(&relation);
        return EXIT_FAILURE;
    }

    printf("Loaded %d relationships.\n", relationship_count);

    for (;;) {
        printf("\n1. Find all products for a customer ID\n");
        printf("2. Find all customers for a product ID\n");
        printf("0. Exit\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            fprintf(stderr, "Invalid input.\n");
            destroy_relation(&relation);
            return EXIT_FAILURE;
        }

        if (choice == 0) {
            break;
        }

        if (choice == 1) {
            printf("Enter customer ID: ");

            if (scanf("%d", &id) != 1) {
                fprintf(stderr, "Invalid customer ID.\n");
                destroy_relation(&relation);
                return EXIT_FAILURE;
            }

            print_products_for_customer(&relation, id);
        } else if (choice == 2) {
            printf("Enter product ID: ");

            if (scanf("%d", &id) != 1) {
                fprintf(stderr, "Invalid product ID.\n");
                destroy_relation(&relation);
                return EXIT_FAILURE;
            }

            print_customers_for_product(&relation, id);
        } else {
            printf("Please enter 0, 1, or 2.\n");
        }
    }

    destroy_relation(&relation);
    return EXIT_SUCCESS;
}
