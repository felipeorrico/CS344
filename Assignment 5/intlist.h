#ifndef IntListH
#define IntListH

#include <stdlib.h>

struct NodeInt {
    int val;
    struct NodeInt* next;
};

struct IntList {
    struct NodeInt* head;
    struct NodeInt* tail;
    int length;
};

struct IntList* intlist_create();
void intlist_clear(struct IntList* list);
int intlist_push_back(struct IntList* list, int val);
void intlist_delete(struct IntList* list);
int intlist_removeNode(struct IntList* list, int index);
int intlist_removeNode_search(struct IntList* list, int value);

#endif