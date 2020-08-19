#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "kallocator.h"
#include "llist.h"
#include <string.h>
#include <stdbool.h>



//creates the struct that contains the memory block with method type and linked lists
struct KAllocator {
    enum allocation_algorithm aalgorithm;
    int size;
    void *memory;
    nodeStruct *freeList;
    nodeStruct *allocatedList;
};

struct KAllocator kallocator;


//initializes all values and creates free linked list pointing to NULL with next and start of memory block with _block
void initialize_allocator(int _size, enum allocation_algorithm _aalgorithm) {
    assert(_size > 0);
    kallocator.aalgorithm = _aalgorithm;
    kallocator.size = _size;
    kallocator.memory = malloc((size_t)kallocator.size);
    nodeStruct *temp = makeNode(kallocator.memory, _size);
    insertAtListHead(&kallocator.freeList, temp);
}

//frees the memory that was created as well as clears the two linked lists
void destroy_allocator() {
    free(kallocator.memory);
    nodeStruct *destroyFree = kallocator.freeList;
    while (destroyFree != NULL) {
	nodeStruct *temp = destroyFree -> next;
	free(destroyFree);
	destroyFree = temp;
    }
    nodeStruct *destroyAllocated = kallocator.allocatedList;
    while (destroyAllocated != NULL) {
	nodeStruct *temp2 = destroyAllocated -> next;
	free(destroyAllocated);
	destroyAllocated = temp2;
    }
}


//forwards to correct method and returns the pointer to the block address of where the memory was allocated
void* kalloc(int _size) {
    void* ptr = NULL;
    switch(kallocator.aalgorithm) {
	case FIRST_FIT:
	    ptr = firstFit(_size);
	    break;
	case BEST_FIT:
	    ptr = bestFit(_size);
	    break;
	case WORST_FIT:
	    ptr = worstFit(_size);
	    break;
	default:
	    printf("Error: Invalid allocator algorithm.\n");
    }
    return ptr;
}


//removes a node from the allocated linked list and adds the same size node to the free linked list while updating memory block
void kfree(void* _ptr) {
    assert(_ptr != NULL);
    nodeStruct *requestedNode = findNodeInList(kallocator.allocatedList, _ptr); //find node to remove from allocated list
    nodeStruct *identicalNode = makeNode(requestedNode->block, requestedNode->size); //create a new identical node to it
    nodeStruct *previousNode = insertAtListTail(&kallocator.freeList, identicalNode); //insert identical node into free list
    mergeNodesInList(previousNode, identicalNode); //merge the newly created node with the free memory block linked list
    deleteNodeFromList(&kallocator.allocatedList, requestedNode); //delete the node from the allocated memory linked list
}


//
int compact_allocation(void** _before, void** _after) {
    int pointersInserted = 0;
    int totalBlockSize = 0;
    checkLists(&(kallocator.freeList));
    checkLists(&(kallocator.allocatedList));
    nodeStruct *currentFreeBlocks = kallocator.freeList;
    nodeStruct *currentAllocatedBlocks = kallocator.allocatedList;
    if (currentFreeBlocks == NULL) {
	return pointersInserted;
    } else {
	while(currentAllocatedBlocks != NULL) {
		if (currentAllocatedBlocks->block == (char *)(kallocator.memory)+totalBlockSize) { //check to see if the block is free or filled
			totalBlockSize += currentAllocatedBlocks->size; //adds to block size to forward the address for next loop iteration
		} else { //block does not match address which means it is empty
			memcpy(currentFreeBlocks->block, currentAllocatedBlocks->block, currentAllocatedBlocks->size); //copies the empty space back over to the free list memory space
			_before[pointersInserted] = currentAllocatedBlocks->block; //records the previous address of the moved block
			currentAllocatedBlocks->block = currentFreeBlocks->block; //moves pointer of currentFreeBlocks memory space
			currentFreeBlocks->block = currentAllocatedBlocks->block + currentAllocatedBlocks->size; //moves the pointer of the free blocks ahead the appropriate amount of memory space
			_after[pointersInserted] = currentAllocatedBlocks->block; //records the new address of the moved block
			totalBlockSize += currentAllocatedBlocks->size; //updates the amount of blocks moved
			pointersInserted += 1; //updates amount of pointers inserted
		}
		currentAllocatedBlocks = currentAllocatedBlocks->next; //moves onto the next node in the list (will analyze it's memory block reference in next loop)
	}
	checkLists(&kallocator.freeList); //maintains correct linked list order of memory
	nodeStruct *present = kallocator.freeList;
	nodeStruct *future = kallocator.freeList->next;
	while(future != NULL) { //checks the free list for blocks and merges them if there is more than 1
		mergeNodesInList(present, future);
		future = future->next;
	}
    }
    return pointersInserted;
}


//sums up the amount of free memory
int available_memory() {
    int available_memory_size = 0;
    nodeStruct * temp = kallocator.freeList;
    while(temp != NULL) {
	available_memory_size += temp->size;
	temp = temp->next;
    }
    return available_memory_size;
}


//calculates and displays the appropriate statistics of the memory block and lists
void print_statistics() {
    int allocated_size = 0;
    int allocated_chunks = 0;
    int free_size = 0;
    int free_chunks = 0;
    int smallest_free_chunk_size = kallocator.size;
    int largest_free_chunk_size = 0;

    nodeStruct *temp = kallocator.allocatedList;
    while (temp != NULL) {
	allocated_size += temp->size;
	allocated_chunks++;
	temp = temp->next;
    }
    free_size = kallocator.size - allocated_size;
    temp = kallocator.freeList;
    while(temp != NULL) {
	free_chunks += 1;
	if (temp->size < smallest_free_chunk_size) {
	    smallest_free_chunk_size = temp->size;
	}
	if (temp->size > largest_free_chunk_size) {
	    largest_free_chunk_size = temp->size;
	}
	temp = temp->next;
    }

    printf("Allocated size = %d\n", allocated_size);
    printf("Allocated chunks = %d\n", allocated_chunks);
    printf("Free size = %d\n", free_size);
    printf("Free chunks = %d\n", free_chunks);
    printf("Largest free chunk size = %d\n", largest_free_chunk_size);
    printf("Smallest free chunk size = %d\n", smallest_free_chunk_size);
}


//allocates the memory on the block by input size at the given block address of the node found by firstfit, bestfit, or worstfit
void allocateMemory(nodeStruct *node, void** ptr, int size) {
    nodeStruct *allocatedMemory = makeNode(node->block, size); //creates identical node
    insertAtListTail(&kallocator.allocatedList, allocatedMemory); //inserts node at end of allocated linked list
    *ptr = node->block; //changes value of pointer to point to the address on the block where memory was allocated
    node->block = ((char *)(node->block)+size); //moves the block address of the free node forward by appropriate amount

    if (node->size > size) {
	node->size -= size; //updates the free node size
    } else {
	deleteNodeFromList(&kallocator.freeList, node); //delete node if it isn't large enough for the requested size (free it)
    }
}


//method for finding the best fit by searching through the free list
void* firstFit(int size) {
    bool enoughSpace = false;
    nodeStruct *temp = kallocator.freeList;
    void* ptr = NULL;

    while (temp != NULL && !enoughSpace) {
	if (temp->size >= size) {
	    enoughSpace = true;
	}

	if (!enoughSpace) {
	    temp = temp->next;
	}
    }

    if (temp == NULL) {
	return ptr;
    } else {
	allocateMemory(temp, &ptr, size);
    }

    return ptr;
}


//method for finding the best fit by searching the entire free list and tracking the smallest possible spot
void* bestFit(int size) {
    void* ptr = NULL;
    int bestSize = kallocator.size;
    nodeStruct *temp = kallocator.freeList;
    nodeStruct *bestSpot = NULL;

    while(temp != NULL) {
	if (temp->size >= size && (temp->size)-size <= bestSize) {
	    bestSpot = temp;
	    bestSize = (temp->size)-size;
	}
	temp = temp->next;
    }
	
    if (bestSpot == NULL) {
	return ptr;
    } else {
	allocateMemory(bestSpot, &ptr, size);
    }

    return ptr;
}


//method for finding the worst fit by searching the entire free list and tracking the largest possible spot
void* worstFit (int size) {
    void* ptr = NULL;
    int worstSize = 0;
    nodeStruct *temp = kallocator.freeList;
    nodeStruct *worstSpot = NULL;

    while(temp != NULL) {
	if (temp->size >= size && (temp->size)-size >= worstSize) {
	    worstSpot = temp;
	    worstSize = (temp->size)-size;
	}
	temp = temp->next;
    }

    if (worstSpot == NULL) {
	return ptr;
    } else {
	allocateMemory(worstSpot, &ptr, size);
    }

    return ptr;
}

