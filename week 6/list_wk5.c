// list_wk5.c
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list_wk5.h"

List new_list() {
	List temp;
	temp.head = NULL;
	return temp;
}

// after changing to char*:
// -- need to change printf for string
void print_list(List *self) {
	ListNodePtr current = self->head;
	while (current != NULL) {
		printf("%s", current->data);
		current = current->next;

		if (current != NULL)
			printf(", ");
	}
	printf("\n");
}

// after changing to char*:
// -- check how long data parameter is (strlen)
// -- allocate just enough memory for newNode->data (malloc)
// -- copy string from parameter into newNode->data (strcpy)
void insert_at_front(List *self, String data) {
	ListNodePtr new_node = malloc(sizeof *new_node);
	size_t len = strlen(data);

	new_node->data = malloc(len + 1);
	strcpy(new_node->data, data);

	new_node->next = self->head;
	self->head = new_node;
}

// after changing to char*:
// -- change test in if statement to string compare (strcmp)
// -- free current->data (memory allocated for string) before freeing current
void delete_from_list(List *self, String data) {
	ListNodePtr current = self->head;
	ListNodePtr prev = NULL;

	while (current != NULL) {
		if (strcmp(current->data, data) == 0) {
			if (prev == NULL) {        // front of list
				self->head = current->next;
				free(current->data);
				free(current);
				current = self->head;
			} else {                    // middle of list
				prev->next = current->next;
				free(current->data);
				free(current);
				current = prev->next;
			}
		} else {
			prev = current;
			current = current->next;
		}
	}
}

// after changing to char*:
// -- need to free memory allocated for string before freeing node
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

static void assert_list_equals(List *self, String expected[], int expected_count) {
	ListNodePtr current = self->head;

	for (int i = 0; i < expected_count; i++) {
		assert(current != NULL);
		assert(strcmp(current->data, expected[i]) == 0);
		current = current->next;
	}

	assert(current == NULL);
}

void list_test() {
	List my_list = new_list();
	String expected_insert[] = {"pear", "orange", "banana", "apple"};

	insert_at_front(&my_list, "apple");
	insert_at_front(&my_list, "banana");
	insert_at_front(&my_list, "orange");
	insert_at_front(&my_list, "pear");
	assert_list_equals(&my_list, expected_insert, 4);

	delete_from_list(&my_list, "banana");
	String expected_delete_middle[] = {"pear", "orange", "apple"};
	assert_list_equals(&my_list, expected_delete_middle, 3);

	delete_from_list(&my_list, "pear");
	String expected_delete_front[] = {"orange", "apple"};
	assert_list_equals(&my_list, expected_delete_front, 2);

	delete_from_list(&my_list, "dragonfruit");
	assert_list_equals(&my_list, expected_delete_front, 2);

	destroy_list(&my_list);
	assert(my_list.head == NULL);

	printf("list_test passed\n");
}
