#ifndef LinkedListH
#define LinkedListH

#include <stdlib.h>

struct Node {
    void* val;
    struct Node* next;
};

struct LinkedList {
    struct Node* head;
    struct Node* tail;
    int length;
};

struct LinkedList* linkedlist_create();
void linkedlist_clear(struct LinkedList* list);
int linkedlist_push_back(struct LinkedList* list, void* val);
void linkedlist_delete(struct LinkedList* list);
struct LinkedList* linkedlist_copy_str(struct LinkedList* list);
void linkedlist_free_dynamic_val(struct LinkedList* list);

#endif