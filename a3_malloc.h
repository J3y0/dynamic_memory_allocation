#ifndef A3_MALLOC
#define A3_MALLOC

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define HEAP_SIZE 4096
#define NODE_OFFSET sizeof(struct h_Node)

// Structure to represent a block in the heap
typedef struct h_Node h_Node;
struct h_Node{
    int STATUS; // status of the block: 0 if it is free and 1 if it is blocked
    size_t SIZE; // size of the block of data in bytes
    void *c_blk; // starting address of the current block of data
    void *n_blk; // starting address of the next block of data
    struct h_Node *NEXT; // points to the next h_Node containing the data for the next block linked to this one
};

// ------- Prototypes of function --------
// Initialize a heap area to use
int m_init(void);
// Allocate a block in the heap of size
void *m_malloc(size_t size);
// De-allocate a block pointed by ptr (if valid)
void m_free(void *ptr);
// Re allocate a block of new size equal to size (ptr is freed and returns a new ptr)
void *m_realloc(void *ptr, size_t size);

// Display all blocks information contained in the heap
void h_layout(struct h_Node *ptr);

#endif