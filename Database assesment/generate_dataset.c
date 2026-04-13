#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "generate_dataset.h"

#define NUM_CUSTOMERS 1000
#define NUM_PRODUCTS 500
#define NUM_RELATIONSHIPS 10000

int generate_data(void) {
    FILE *file = fopen("Database assesment/data.txt", "w");

    if (file == NULL) {
        file = fopen("data.txt", "w");
    }

    if (file == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    srand(time(NULL));

    for (int i = 0; i < NUM_RELATIONSHIPS; i++) {
        int customerID = rand() % NUM_CUSTOMERS;
        int productID = rand() % NUM_PRODUCTS;

        fprintf(file, "%d %d\n", customerID, productID);
    }

    fclose(file);

    printf("Data generated successfully.\n");
    return 0;
}
