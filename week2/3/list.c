#include <stdio.h>
#include <stdlib.h>
#include "list.h"


// Function to create and return a new empty list.
List new_list() {
	List temp;
	temp.head = NULL;
	return temp;
}

void option_insert(List* myList){
    int x;

    printf("Current list: ");
    print_list(myList);

    printf("int to insert: ");
    scanf("%d", &x);

    insert_at_front(myList, x);

    printf("list after insertion: ");
    print_list(myList);
}

void option_delete(List* myList){
    int x;
    printf("Current list: ");
    print_list(myList);

    printf("int to delete: ");
    scanf("%d", &x);

    delete_list(myList, x);

    printf("list after deletion: ");
    print_list(myList);
}

void option_print(List* myList){
    printf("current list: ");
    print_list(myList);
}

void list_adhoc_test(){
    List myList = new_list();
    int quit = 0;

    while (!quit)
    {
        int option;
        printf("Select an option: \n 0: Quit \n 1: Insert \n 2: Delete \n 3: Print \n");

        scanf("%d", &option);

        if (option == 0){
            quit = 1;
        }
        else if (option == 1)
        {
            option_insert(&myList);  // pass address
        }
        else if (option == 2)
        {
            option_delete(&myList);  // pass address
        }
        else if (option == 3)
        {
            option_print(&myList);   // pass address
        }
        else{
            printf("error, invalid selection \n");
        }
    }

    destroy_list(&myList);
}

void reverse_list(List* self){
	ListNodePtr current = self -> head;
	ListNodePtr prev = NULL;
	ListNodePtr next = NULL;

	while (current != NULL){
		next = current->next;
		current->next = prev;
		prev = current;
		current = next;
	}

	self -> head = prev; 
}

void merge_lists(List* l1, List* l2) {
    ListNodePtr c1 = l1->head;
    ListNodePtr c2 = l2->head;
    ListNodePtr prev = NULL;

    while (c2 != NULL) {
        
        if (c1 == NULL || c2->data <= c1->data) {
            ListNodePtr next2 = c2->next;  
    
            c2->next = c1;
            if (prev == NULL) {
                l1->head = c2;  
            } else {
                prev->next = c2;
            }

            prev = c2;
            c2 = next2;
        } else {
            prev = c1;
            c1 = c1->next;
        }
    }
}



// Function to print the contents of a list.
void print_list(List* self) {
	ListNodePtr current = self->head;
	while (current != NULL) {
		printf("%d", current->data);
		current = current->next;

		if (current != NULL)
			printf(", ");
	}
	printf("\n");
}


// Function to insert a new node with given data at the front of the list.
void insert_at_front(List* self, int data) {
	ListNodePtr new_node = malloc(sizeof * new_node);

	new_node->data = data;
	new_node->next = self->head;
	self->head = new_node;
}

// Function to insert a new node with given data in the list in ascending order.
void insert_in_order(List* self, int data) {
	ListNodePtr current = self->head;
	ListNodePtr prev = NULL;

	ListNodePtr new_node = malloc(sizeof * new_node);
	new_node->data = data;
	new_node->next = NULL;

	while (current != NULL && current->data < data) {
		prev = current;
		current = current->next;
	}

	if (current == self->head) { // at front
		new_node->next = current;
		self->head = new_node;
	}
	else {                     // middle
		new_node->next = current;
		prev->next = new_node;
	}
}

// Function to delete the first occurrence of a node with given data from the list.
void delete_list(List* self, int data) {
	ListNodePtr current = self->head;
	ListNodePtr prev = NULL;

	while (current != NULL) {
		if (current->data == data) {
			if (prev == NULL) {        // front of list
				self->head = current->next;
				free(current);
				current = self->head;
			}
			else {                    // middle of list
				prev->next = current->next;
				free(current);
				current = prev->next;
			}
		}
		else {
			prev = current;
			current = current->next;
		}
	}
}

// Function to free the memory allocated to a list and all its nodes.
void destroy_list(List* self) {
	ListNodePtr current = self->head;
	while (current != NULL) {
		ListNodePtr to_free = current;
		current = current->next;
		free(to_free);
	}
	self->head = NULL;
}