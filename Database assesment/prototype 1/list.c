#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

List new_list(void) {
    List temp;
    temp.head = NULL;
    return temp;
}

void print_list(List *self) {
    ListNodePtr current = self->head;

    while (current != NULL) {
        printf("%s", current->data);
        current = current->next;

        if (current != NULL) {
            printf(", ");
        }
    }

    printf("\n");
}

void insert_at_front(List *self, String data) {
    ListNodePtr new_node = malloc(sizeof *new_node);
    size_t len;

    if (new_node == NULL) {
        fprintf(stderr, "Error allocating list node.\n");
        exit(EXIT_FAILURE);
    }

    len = strlen(data);
    new_node->data = malloc(len + 1);

    if (new_node->data == NULL) {
        free(new_node);
        fprintf(stderr, "Error allocating list string.\n");
        exit(EXIT_FAILURE);
    }

    strcpy(new_node->data, data);
    new_node->next = self->head;
    self->head = new_node;
}

void delete_from_list(List *self, String data) {
    ListNodePtr current = self->head;
    ListNodePtr prev = NULL;

    while (current != NULL) {
        if (strcmp(current->data, data) == 0) {
            if (prev == NULL) {
                self->head = current->next;
            } else {
                prev->next = current->next;
            }

            free(current->data);
            free(current);

            if (prev == NULL) {
                current = self->head;
            } else {
                current = prev->next;
            }
        } else {
            prev = current;
            current = current->next;
        }
    }
}

void destroy_list(List *self) {
    ListNodePtr current = self->head;

    while (current != NULL) {
        ListNodePtr to_free = current;

        current = current->next;
        free(to_free->data);
        free(to_free);
    }

    self->head = NULL;
}
