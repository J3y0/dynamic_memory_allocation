#include "a3_malloc.h"

h_Node *heap = NULL;

void h_layout(struct h_Node *ptr) {
    h_Node *current_node = ptr;
    int i = 1;
    printf("----------BEGINNING-OF-HEAP----------\n");
    while (current_node) {
        printf("-------BLOCK-%d-------\n", i);
        printf("BLOCK AT ADDRESS: %p\n", current_node);
        printf("Is allocated: %d\n", current_node->STATUS);
        printf("Size of block: 0x%lx (decimal: %ld)\n", current_node->SIZE, current_node->SIZE);
        printf("Address of current block data: %p\n", current_node->c_blk);
        printf("Address of next block data: %p\n", current_node->n_blk);

        // Update current node to the next one
        current_node = current_node->NEXT;
        ++i;
    }
    printf("----------END-OF-HEAP----------------\n\n");
}

int m_init(void) {
    void *ptr_to_heap = sbrk(0);

    if (brk(ptr_to_heap + NODE_OFFSET + HEAP_SIZE) == -1) {
        return 1;
    }

    heap = (h_Node *)ptr_to_heap;
    heap->c_blk = ptr_to_heap + NODE_OFFSET;
    heap->n_blk = NULL;
    heap->NEXT = NULL;
    heap->SIZE = HEAP_SIZE;
    heap->STATUS = 0;

    // Successfull initialization
    return 0;
}

void *m_malloc(size_t size) {
    void *ptr = NULL;
    h_Node *current_node = heap;
    h_Node *previous_node = NULL;

    // Find next free block if any
    while (current_node) {
        // If block is unoccupied
        if (!current_node->STATUS) {
            // Check the size is sufficient
            if (size + NODE_OFFSET < current_node->SIZE) {
                // Should split the block
                h_Node *new_node = (h_Node *)(current_node->c_blk + size);
                new_node->STATUS = 0;
                new_node->SIZE = current_node->SIZE - size - NODE_OFFSET;
                new_node->n_blk = current_node->n_blk;
                new_node->NEXT = current_node->NEXT;
                new_node->c_blk = current_node->c_blk + size + NODE_OFFSET;
                
                // Update current_node
                current_node->n_blk = new_node->c_blk;
                current_node->NEXT = new_node;
                current_node->STATUS = 1;
                current_node->SIZE = size;
                ptr = current_node->c_blk;
                break;
            } else if (size <= current_node->SIZE) {
                current_node->STATUS = 1;
                ptr = current_node->c_blk;
                break;
            }
        }
        previous_node = current_node;
        current_node = current_node->NEXT;
    }

    // If no blocks were found, extend the heap
    if (ptr == NULL) {
        // If last block is occupied, add an entire new block
        if (previous_node->STATUS) {
            ptr = sbrk(0);
            if (brk(ptr + NODE_OFFSET + size) == -1) {
                return NULL; // Unsuccessfull allocation
            }
            h_Node *new_node = (h_Node *)ptr;
            new_node->STATUS = 1;
            new_node->SIZE = size;
            new_node->n_blk = NULL;
            new_node->NEXT = NULL;
            new_node->c_blk = ptr + NODE_OFFSET;

            // Update last node
            previous_node->NEXT = new_node;
            previous_node->n_blk = new_node->c_blk;
        } else {
            // Else just extend the last block
            if (brk(sbrk(0) + size - previous_node->SIZE) == -1) {
                return NULL; // Unsuccessfull allocation
            }

            previous_node->STATUS = 1;
            previous_node->SIZE = size;
            ptr = previous_node->c_blk;
        }
    }

    return ptr;
}

int m_check(void) {
    h_Node *current_node = heap;
    h_Node *previous_node = NULL;
    int index = 0;

    // Go through entire Heap
    while (current_node) {
        current_node->STATUS = 0;

        // Check if following block can be merged
        h_Node *next_block = current_node->NEXT;
        if ((next_block != NULL) && (!next_block->STATUS)) {
            printf("NEXT_BLOCK can be merged with CURRENT_BLOCK number: %d\n", index);
            return -1;
        }

        // Check if previous block can be merged
        if ((previous_node != NULL) && (!previous_node->STATUS)) {
            printf("PREVIOUS_BLOCK can be merged with CURRENT_BLOCK number: %d\n", index);
            return -1;
        }
        previous_node = current_node;
        current_node = current_node->NEXT;
        index++;
    }
    return 0;
}

void m_free(void *ptr) {
    if (ptr == NULL) {
        perror("[m_free] Error: pointer passed as parameter is NULL.");
        exit(1);
    }

    h_Node *current_node = heap;
    h_Node *previous_node = NULL;
    int unsuccessful = 1;
    // Find the block corresponding to the address given
    while (current_node) {
        if (ptr == current_node->c_blk) {
            unsuccessful = 0;
            current_node->STATUS = 0;

            // Merge the following block if also unoccupied
            h_Node *next_block = current_node->NEXT;
            if ((next_block != NULL) && (!next_block->STATUS)) {
                current_node->SIZE = current_node->SIZE + next_block->SIZE + NODE_OFFSET;
                current_node->NEXT = next_block->NEXT;
                current_node->n_blk = next_block->n_blk;
            }

            // Merge the previous block if also unoccupied
            if ((previous_node != NULL) && (!previous_node->STATUS)) {
                previous_node->NEXT = current_node->NEXT;
                previous_node->n_blk = current_node->n_blk;
                previous_node->SIZE = previous_node->SIZE + current_node->SIZE + NODE_OFFSET;
            }
            break;
        }
        previous_node = current_node;
        current_node = current_node->NEXT;
    }

    if (unsuccessful) {
        printf("Given pointer is not valid.");
    }
}

void *m_realloc(void *ptr, size_t size) {
    if (ptr == NULL) {
        return m_malloc(size);
    }
    if (size == 0) {
        m_free(ptr);
        return NULL;
    }

    // Allocate another block with new size
    void *new_ptr = m_malloc(size);
    if (new_ptr == NULL) {
        return NULL; // Unsuccessful allocation
    }

    // Find the minimal size
    int old_size = 0;
    h_Node *current_node = heap;
    while (current_node) {
        if (ptr == current_node->c_blk) {
            old_size = current_node->SIZE;
            break;
        }
        current_node = current_node->NEXT;
    }

    int min_size = old_size < size ? old_size:size;

    // Copy existing data to new location
    memcpy(new_ptr, ptr, min_size);

    m_free(ptr);
    return new_ptr;
}

int main() {
    if (m_init()) {
        perror("Failed to initialize heap area.\n");
        return 1;
    }
    printf("h_Node offset: 0x%lx (decimal: %ld)\n\n", NODE_OFFSET, NODE_OFFSET);

    // Initial heap
    h_layout(heap);

    int *ptr_int = m_malloc(sizeof(int));
    if (ptr_int == NULL) {
        printf("Error while allocating memory with m_malloc.");
        return 1;
    }
    h_layout(heap);
    *ptr_int = 4;

    double *new_ptr_dbl = m_realloc(ptr_int, sizeof(double));
    *new_ptr_dbl = 4.0;
    h_layout(heap);

    double *another_dbl = m_malloc(sizeof(double));
    *another_dbl = 3.2;

    h_layout(heap);

    char *c = m_malloc(sizeof(char));
    *c = 'a';

    h_layout(heap);
    
    m_free(new_ptr_dbl);
    h_layout(heap);

    m_free(c);
    h_layout(heap);

    m_free(another_dbl);
    h_layout(heap);

    return 0;
}