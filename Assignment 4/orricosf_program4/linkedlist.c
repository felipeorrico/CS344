
#include "linkedlist.h"
#include <string.h>

/*********************************************************************
** Function: linkedlist_create
** Description: Creates and allocates memory for a LinkedList
*********************************************************************/
struct LinkedList* linkedlist_create(){
    struct LinkedList* list = (struct LinkedList*) malloc(sizeof(struct LinkedList));
    list->head = NULL;
    list->length = 0;

    return list;
}

/*********************************************************************
** Function: linkedlist_clear
** Description: Frees the memory of all the nodes of the LinkedList,
** but does not free values within the nodes and the list itself.
*********************************************************************/
void linkedlist_clear(struct LinkedList* list)
{
	struct Node* next = list->head;
	struct Node* cur = NULL;
	while (next != NULL) {
		cur = next;
		next = next->next;
		free(cur);
	}
	list->length = 0;
}

/*********************************************************************
** Function: linkedlist_delete
** Description: Frees memory of nodes and LinkedList, but not from
** node values.
*********************************************************************/
void linkedlist_delete(struct LinkedList* list)
{
	struct Node* next = list->head;
	struct Node* cur = NULL;
	while (next != NULL) {
		cur = next;
		next = next->next;
		free(cur);
	}
	list->length = 0;
	free(list);
}

/*********************************************************************
** Function: linkedlist_push_back
** Description: Pushed val to the end of the linked list.
*********************************************************************/
int linkedlist_push_back(struct LinkedList* list, void* val)
{
	struct Node* node = list->head;
	if (!node) {
		node = (struct Node*) malloc(sizeof(struct Node));
		list->head = node;
        list->tail = node;
	}
	else {
        struct Node* tail = list->tail;
		tail->next = (struct Node*) malloc(sizeof(struct Node));
		list->tail = tail->next;
	}
	list->tail->val = val;
	list->tail->next = 0;
	list->length++;

    return list->length;
}

struct LinkedList* linkedlist_copy_str(struct LinkedList* list){
	struct LinkedList* out = linkedlist_create();

	struct Node* node = list->head;
	while (node){
		char* arr = malloc(sizeof(char) * (strlen(node->val) + 1));
		strcpy(arr, node->val);
		linkedlist_push_back(out, arr);
		node = node->next;
	}

	return out;
}

void linkedlist_free_dynamic_val(struct LinkedList* list){
    if (!list)
        return;
	struct Node* node = list->head;
	while (node){
		free(node->val);
		node = node->next;
	}
	linkedlist_delete(list);
	list = NULL;
}