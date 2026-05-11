#include <stdio.h>


#include <stdlib.h>
#include <string.h>

// typedef for readability
typedef char *String;

int main() {
    int n;

    printf("Enter number of strings: ");
    scanf("%d", &n);

    String *strings;

    strings = malloc(n * sizeof(String));

    char buffer[100];  

    for (int i = 0; i < n; i++) {
        printf("Enter string %d: ", i + 1);
        scanf("%s", buffer);

        strings[i] = malloc(strlen(buffer) + 1);

        strcpy(strings[i], buffer);
    }

    printf("\nYou entered:\n");
    for (int i = 0; i < n; i++) {
        printf("%s\n", strings[i]);
    }

    for (int i = 0; i < n; i++) {
        free(strings[i]);
    }

    free(strings);

    return 0;
}

void quicksort(String *a, int first, int last) {
    if (first < last) {
        int i = first;
        int j = last - 1;
        String pivot = a[last];

        while (i <= j) {
            while (i < last && strcmp(a[i], pivot) <= 0) {
                i++;
            }

            while (j >= first && strcmp(a[j], pivot) > 0) {
                j--;
            }

            if (i < j) {
                String temp = a[i];
                a[i] = a[j];
                a[j] = temp;
            }
        }

        String temp = a[i];
        a[i] = a[last];
        a[last] = temp;

        quicksort(a, first, j);
        quicksort(a, i + 1, last);
    }
}