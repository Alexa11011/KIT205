#pragma once

typedef char *String;

typedef struct listNode {
    String data;
    struct listNode *next;
} *ListNodePtr;

typedef struct list {
    ListNodePtr head;
} List;

List new_list(void);
void print_list(List *self);
void insert_at_front(List *self, String data);
void delete_from_list(List *self, String data);
void destroy_list(List *self);
