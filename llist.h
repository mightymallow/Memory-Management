#ifndef LLIST_H_
#define LLIST_H_

typedef struct nodeStruct {
    void *block;
    int size;
    struct nodeStruct *next;
} nodeStruct;

struct nodeStruct* makeNode(void *memoryBlock, int size);
void insertAtListHead (struct nodeStruct **headRef, struct nodeStruct *node);
nodeStruct* insertAtListTail (struct nodeStruct **headRef, struct nodeStruct *node);
struct nodeStruct* findNodeInList(struct nodeStruct *allocatedListHead, void* _block);
void mergeNodesInList(nodeStruct *last, nodeStruct *newer);
void deleteNodeFromList (struct nodeStruct **headRef, struct nodeStruct *node);
void checkLists(struct nodeStruct **headRef);
void swapNodes(struct nodeStruct **pastNode, struct nodeStruct *baseNode, struct nodeStruct *futureNode);
int countAllNodes (struct nodeStruct *list);


#endif
