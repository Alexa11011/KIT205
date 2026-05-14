#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "prototype 1/relation_for_list.h"
#include "prototype 2/relation_for_avl.h"
#include "tests.h"

static const char *find_data_filename(void) {
    FILE *file = fopen("Database assesment/data.txt", "r");

    if (file != NULL) {
        fclose(file);
        return "Database assesment/data.txt";
    }

    file = fopen("data.txt", "r");

    if (file != NULL) {
        fclose(file);
        return "data.txt";
    }

    return NULL;
}

static double seconds_between(clock_t start, clock_t end) {
    return (double) (end - start) / CLOCKS_PER_SEC;
}

static int load_default_list_relation(ListManyToManyRelation *relation) {
    int count = load_list_relationships_from_file(relation, "Database assesment/data.txt");

    if (count >= 0) {
        return count;
    }

    return load_list_relationships_from_file(relation, "data.txt");
}

static int load_default_avl_relation(ManyToManyRelation *relation) {
    int count = load_relationships_from_file(relation, "Database assesment/data.txt");

    if (count >= 0) {
        return count;
    }

    return load_relationships_from_file(relation, "data.txt");
}

static void run_list_lookup_menu(void) {
    ListManyToManyRelation relation = create_list_relation(1009, 509);
    int relationship_count = load_default_list_relation(&relation);
    int choice;
    int id;

    if (relationship_count < 0) {
        fprintf(stderr, "Could not load relationship data from Database assesment/data.txt or data.txt\n");
        destroy_list_relation(&relation);
        return;
    }

    printf("Prototype 1 loaded %d relationships.\n", relationship_count);

    for (;;) {
        printf("\nPrototype 1: hash table into linked list\n");
        printf("1. Find all products for a customer ID\n");
        printf("2. Find all customers for a product ID\n");
        printf("0. Back\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            fprintf(stderr, "Invalid input.\n");
            destroy_list_relation(&relation);
            exit(EXIT_FAILURE);
        }

        if (choice == 0) {
            break;
        }

        if (choice == 1) {
            printf("Enter customer ID: ");

            if (scanf("%d", &id) != 1) {
                fprintf(stderr, "Invalid customer ID.\n");
                destroy_list_relation(&relation);
                exit(EXIT_FAILURE);
            }

            print_list_products_for_customer(&relation, id);
        } else if (choice == 2) {
            printf("Enter product ID: ");

            if (scanf("%d", &id) != 1) {
                fprintf(stderr, "Invalid product ID.\n");
                destroy_list_relation(&relation);
                exit(EXIT_FAILURE);
            }

            print_list_customers_for_product(&relation, id);
        } else {
            printf("Please enter 0, 1, or 2.\n");
        }
    }

    destroy_list_relation(&relation);
}

static void run_avl_lookup_menu(void) {
    ManyToManyRelation relation = create_relation(1009, 509);
    int relationship_count = load_default_avl_relation(&relation);
    int choice;
    int id;

    if (relationship_count < 0) {
        fprintf(stderr, "Could not load relationship data from Database assesment/data.txt or data.txt\n");
        destroy_relation(&relation);
        return;
    }

    printf("Prototype 2 loaded %d relationships.\n", relationship_count);

    for (;;) {
        printf("\nPrototype 2: hash table into AVL\n");
        printf("1. Find all products for a customer ID\n");
        printf("2. Find all customers for a product ID\n");
        printf("0. Back\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            fprintf(stderr, "Invalid input.\n");
            destroy_relation(&relation);
            exit(EXIT_FAILURE);
        }

        if (choice == 0) {
            break;
        }

        if (choice == 1) {
            printf("Enter customer ID: ");

            if (scanf("%d", &id) != 1) {
                fprintf(stderr, "Invalid customer ID.\n");
                destroy_relation(&relation);
                exit(EXIT_FAILURE);
            }

            print_products_for_customer(&relation, id);
        } else if (choice == 2) {
            printf("Enter product ID: ");

            if (scanf("%d", &id) != 1) {
                fprintf(stderr, "Invalid product ID.\n");
                destroy_relation(&relation);
                exit(EXIT_FAILURE);
            }

            print_customers_for_product(&relation, id);
        } else {
            printf("Please enter 0, 1, or 2.\n");
        }
    }

    destroy_relation(&relation);
}

static void benchmark_list_relation(void) {
    ListManyToManyRelation relation = create_list_relation(1009, 509);
    clock_t start;
    clock_t end;
    int relationship_count;
    long long read_count;
    double write_seconds;
    double read_seconds;

    start = clock();
    relationship_count = load_default_list_relation(&relation);
    end = clock();
    write_seconds = seconds_between(start, end);

    if (relationship_count < 0) {
        fprintf(stderr, "Could not load relationship data for prototype 1.\n");
        destroy_list_relation(&relation);
        return;
    }

    start = clock();
    read_count = count_list_relation_read_values(&relation);
    end = clock();
    read_seconds = seconds_between(start, end);

    printf("Prototype 1 - hash table into linked list\n");
    printf("  Write/load: %d relationships in %.3f seconds\n", relationship_count, write_seconds);
    printf("  Read/sort:  %lld values prepared in %.3f seconds\n", read_count, read_seconds);

    destroy_list_relation(&relation);
}

static void benchmark_avl_relation(void) {
    ManyToManyRelation relation = create_relation(1009, 509);
    clock_t start;
    clock_t end;
    int relationship_count;
    long long read_count;
    double write_seconds;
    double read_seconds;

    start = clock();
    relationship_count = load_default_avl_relation(&relation);
    end = clock();
    write_seconds = seconds_between(start, end);

    if (relationship_count < 0) {
        fprintf(stderr, "Could not load relationship data for prototype 2.\n");
        destroy_relation(&relation);
        return;
    }

    start = clock();
    read_count = count_relation_read_values(&relation);
    end = clock();
    read_seconds = seconds_between(start, end);

    printf("Prototype 2 - hash table into AVL\n");
    printf("  Write/load: %d relationships in %.3f seconds\n", relationship_count, write_seconds);
    printf("  Read/count: %lld values prepared in %.3f seconds\n", read_count, read_seconds);

    destroy_relation(&relation);
}

static void benchmark_prototypes(void) {
    const char *filename = find_data_filename();

    if (filename == NULL) {
        fprintf(stderr, "Could not load relationship data from Database assesment/data.txt or data.txt\n");
        return;
    }

    printf("Benchmarking with %s\n", filename);
    printf("Console output of relationship IDs is skipped for timing.\n\n");

    benchmark_list_relation();
    printf("\n");
    benchmark_avl_relation();
}

int main(void) {
    int choice;

    run_all_tests();

    for (;;) {
        printf("\n1. Select prototype 1 (hash table into linked list)\n");
        printf("2. Select prototype 2 (hash table into AVL)\n");
        printf("3. Test prototypes against each other\n");
        printf("0. Exit\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            fprintf(stderr, "Invalid input.\n");
            return EXIT_FAILURE;
        }

        if (choice == 0) {
            break;
        } else if (choice == 1) {
            run_list_lookup_menu();
        } else if (choice == 2) {
            run_avl_lookup_menu();
        } else if (choice == 3) {
            benchmark_prototypes();
        } else {
            printf("Please enter 0, 1, 2, or 3.\n");
        }
    }

    return EXIT_SUCCESS;
}
