#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "list.c"

//my func
void insert_at_front_list_test(){
    List myList = new_list();

    printf("testing insert at front");
    

    insert_at_front(&myList, 5);
    insert_at_front(&myList, 3);
    insert_at_front(&myList, 7);
    insert_at_front(&myList, 2);
    insert_at_front(&myList, 0);

    printf("Expected: 0, 2, 7, 3, 5 \n");
    printf("result: ");

    print_list(&myList);
}

void insert_in_order_list_test(){
    List myList = new_list();

    printf("testing insert in order");

    insert_in_order(&myList, 0);
    insert_in_order(&myList, 2);
    insert_in_order(&myList, 7);
    insert_in_order(&myList, 3);
    insert_in_order(&myList, 5);

    printf("expected 0, 2, 3, 5, 7 \n");
    printf("result:");

    print_list(&myList);

}

void delete_list_test(){
    List myList = new_list();


    insert_at_front(&myList, 5);
    insert_at_front(&myList, 3);
    insert_at_front(&myList, 7);

    printf("list prior to deletion test: ");
    print_list(&myList);

    printf("expect result after deletion: 5, 7");
    delete_list(&myList, 3);

    printf("result: ");
    print_list(&myList);

}

void destroy_list_test(){
    List myList = new_list();

    insert_at_front(&myList, 5);

    printf("is list empty, exepted: FALSE: ");
    if (myList.head == NULL){
        printf("TRUE");
    }
    else{
        printf("FALSE");
    }
    printf("\n");
    destroy_list(&myList);


    printf("is list empty, exepted: TRUE: ");
    if (myList.head == NULL){
        printf("TRUE");
    }
    else{
        printf("FALSE");
    }

}



int main(){
    //insert_at_front_list_test();
    //insert_in_order_list_test();
    //delete_list_test();
    destroy_list_test();



}


