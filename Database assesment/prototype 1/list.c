#include <stdio.h>
#include <stdlib.h>

#include "list.h"

List new_list(void) {
    List temp;
    temp.head = NULL;
    return temp;
}

void print_list(List *self) {
    ListNodePtr current = self->head;

    while (current != NULL) {
        printf("%d", current->data);
        current = current->next;

        if (current != NULL) {
            printf(", ");
        }
    }

    printf("\n");
}

void insert_at_front(List *self, int data) {
    ListNodePtr new_node = malloc(sizeof *new_node);

    if (new_node == NULL) {
        fprintf(stderr, "Error allocating list node.\n");
        exit(EXIT_FAILURE);
    }

    new_node->data = data;
    new_node->next = self->head;
    self->head = new_node;
}

void delete_from_list(List *self, int data) {
    ListNodePtr current = self->head;
    ListNodePtr prev = NULL;

    while (current != NULL) {
        if (current->data == data) {
            if (prev == NULL) {
                self->head = current->next;
            } else {
                prev->next = current->next;
            }

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
        free(to_free);
    }

    self->head = NULL;
}
