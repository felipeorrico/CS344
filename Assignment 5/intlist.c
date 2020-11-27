/***********************************************************************
** File: intlist.c
** Author: Felipe Orrico Scognamiglio
** Class: CS344 F2020
** Instructor: Justin Goins
** Assignment: One-Time Pads (Assignment 5)
** Final Version Date: 11/27/2020
***********************************************************************/

#include "intlist.h"

/*********************************************************************
** Function: intlist_create
** Description: Creates and allocates memory for a LinkedList
*********************************************************************/
struct IntList* intlist_create(){
    struct IntList* list = (struct IntList*) malloc(sizeof(struct IntList));
    list->head = NULL;
	list->tail = NULL;
    list->length = 0;
    return list;
}

/*********************************************************************
** Function: intlist_clear
** Description: Frees the memory of all the nodes of the LinkedList,
** but does not free values within the nodes and the list itself.
*********************************************************************/
void intlist_clear(struct IntList* list)
{
	struct NodeInt* next = list->head;
	struct NodeInt* cur = NULL;
	while (next != NULL) {
		cur = next;
		next = next->next;
		free(cur);
	}
	list->length = 0;
    list->head = NULL;
    list->tail = NULL;
}

/*********************************************************************
** Function: intlist_delete
** Description: Frees memory of nodes and LinkedList, but not from
** node values.
*********************************************************************/
void intlist_delete(struct IntList* list)
{
	struct NodeInt* next = list->head;
	struct NodeInt* cur = NULL;
	while (next != NULL) {
		cur = next;
		next = next->next;
		free(cur);
	}
	list->length = 0;
	free(list);
}

/*********************************************************************
** Function: intlist_push_back
** Description: Pushed val to the end of the linked list.
*********************************************************************/
int intlist_push_back(struct IntList* list, int val)
{
	struct NodeInt* node = list->head;
	if (!node) {
		node = (struct NodeInt*) malloc(sizeof(struct NodeInt));
		list->head = node;
        list->tail = node;
	}
	else {
        struct NodeInt* tail = list->tail;
		tail->next = (struct NodeInt*) malloc(sizeof(struct NodeInt));
		list->tail = tail->next;
	}
	list->tail->val = val;
	list->tail->next = 0;
	list->length++;

    return list->length;
}

/*********************************************************************
** Function: intlist_removeNode
** Description: Removes node at index. does not check out of bounds
*********************************************************************/
int intlist_removeNode(struct IntList* list, int index){
	struct NodeInt* node = list->head;
	struct NodeInt* before_node = NULL;
	struct NodeInt* removedNode = NULL;
	int curr_index = 0;
	while(node){
		if ((curr_index + 1) == index){
			before_node = node;
		}
		if (curr_index == index){
			removedNode = node;
			before_node->next = node->next;
			removedNode->next = NULL;
			break;
		}
		node = node->next;
		curr_index++;
	}

	int val = removedNode->val;
	free(removedNode);

	return val;
}

/*********************************************************************
** Function: intlist_removeNode
** Description: search and remove node with value.
*********************************************************************/
int intlist_removeNode_search(struct IntList* list, int value){
	struct NodeInt* node = list->head;
	struct NodeInt* before_node = NULL;
	struct NodeInt* removedNode = NULL;
	if (!node){
		return -5;
	}
	if (list->length == 1){
		int val = list->head->val;
		free(list->head);
		list->head = NULL;
		list->tail = NULL;
		return val;
	}
	while(node){
		
		if (node->val == value){
			removedNode = node;
			if (before_node){
				before_node->next = node->next;
				node->next = NULL;
			}
			else {
				list->head = node->next;
				node->next = NULL;
			}
			break;
		}

		before_node = node;
		node = node->next;
	}

	int val = removedNode->val;
	free(removedNode);

	return val;
}