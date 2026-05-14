#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "generate_dataset.h"

#define NUM_CUSTOMERS 1000000
#define NUM_PRODUCTS 500000
#define TARGET_DATASET_MB 500
#define BYTES_PER_MB 1048576LL

int generate_data(void) {
    FILE *file = fopen("Database assesment/data.txt", "w");
    long long target_bytes = TARGET_DATASET_MB * BYTES_PER_MB;
    long long bytes_written = 0;
    long long relationships_written = 0;

    if (file == NULL) {
        file = fopen("data.txt", "w");
    }

    if (file == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    srand(time(NULL));

    while (bytes_written < target_bytes) {
        int customerID = rand() % NUM_CUSTOMERS;
        int productID = rand() % NUM_PRODUCTS;
        int written = fprintf(file, "%d %d\n", customerID, productID);

        if (written < 0) {
            printf("Error writing data.\n");
            fclose(file);
            return 1;
        }

        bytes_written += written;
        relationships_written++;
    }

    fclose(file);

    printf("Data generated successfully: %lld relationships, about %lld MB.\n",
           relationships_written,
           bytes_written / BYTES_PER_MB);
    return 0;
}
