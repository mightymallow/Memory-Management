#include "llist.h"
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>


//creates a node that is linked to the memory block
struct nodeStruct* makeNode(void *memoryBlock, int size) {
    nodeStruct *temp = malloc(sizeof(struct nodeStruct));
    if (temp != NULL) {
	temp -> block = memoryBlock;
	temp -> size = size;
	temp -> next = NULL;
    }
    return temp;
}


//inserts node a head of list
void insertAtListHead (struct nodeStruct **headRef, struct nodeStruct *node) {
    if (*headRef != NULL) {
	node -> next = *headRef;
	*headRef = node;
    } else {
	*headRef = node;
	node -> next = NULL;
    }
}


//inserts node at tail of list and returns previous node for easy check of possible free memory merge
nodeStruct* insertAtListTail (struct nodeStruct **headRef, struct nodeStruct *node) {
    if (*headRef != NULL) {
	struct nodeStruct *temp = *headRef;
	while (temp->next != NULL) {
	    temp = temp->next;
	}
	temp->next = node;
	return temp;
    } else { 
	*headRef = node;
	return NULL;
    }
}


//searches for a node in the list based on pointer to the memory block
struct nodeStruct* findNodeInList(struct nodeStruct *allocatedListHead, void* _block) {
	struct nodeStruct *temp = allocatedListHead;
	while (temp != NULL) {
		if (temp->block == _block) {
			return temp;
		}
		temp = temp->next;
	}
	return NULL;
}


//merges two nodes in the list
void mergeNodesInList(nodeStruct *last, nodeStruct *newer) {
    if (last != NULL && newer != NULL) {
	last->size += newer->size;
	last->next = newer->next;
	free(newer);
	newer = NULL;
    }
}


//deletes the node from the list and updates pointers
void deleteNodeFromList(struct nodeStruct **headRef, struct nodeStruct *node) {
    if (*headRef != NULL && node != NULL) {
	if (*headRef == node) {
	    *headRef = node->next;
	} else {
	    nodeStruct *temp = *headRef;
	    while (temp->next != node) {
		temp = temp->next;
	    }
	    temp->next = node->next;
	}
	free(node);
    }
}


//checks a linked list to make sure that the memory addresses are ascending
void checkLists(struct nodeStruct **headRef) {
    bool finished = false;
    while (*headRef != NULL && !finished) {
	struct nodeStruct * baseNode = *headRef;
	if (baseNode->next == NULL) {
	    finished = true;
	}

	if (!finished) {
	    struct nodeStruct * futureNode = baseNode->next;
	    if (baseNode->block > futureNode->block){
		swapNodes(headRef, baseNode, futureNode);
	    }
	}
	headRef = &((*headRef)->next);
    }
}


//swaps 2 nodes in a linked list 
void swapNodes(struct nodeStruct **pastNode, struct nodeStruct *baseNode, struct nodeStruct *futureNode) {
	*pastNode = futureNode;
	baseNode->next = futureNode->next;
	futureNode->next = baseNode;
}


//counts amount of nodes in the list (memory chunks)
int countAllNodes (struct nodeStruct *list) {
	int counter = 0;
	struct nodeStruct *temp = list;
	while (temp != NULL) {
		temp = temp->next;
		counter++;
	}
	return counter;
}
    
