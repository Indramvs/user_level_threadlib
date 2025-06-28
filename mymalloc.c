#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include "mymalloc.h"

// Structure to store metadata of each memory block
struct block_metadata {
    size_t size;                        // Size of the user-allocated memory
    struct block_metadata* next;       // Pointer to the next block in the list
    bool free;                          // Indicates if the block is currently free
};

// Head of the linked list of memory blocks
struct block_metadata* global_head = NULL;

// Macro to calculate size of metadata (used for pointer arithmetic)
#define META_SIZE sizeof(struct block_metadata)



// Custom malloc implementation 
void* mymalloc(size_t size) {
    if (size == 0) return NULL;
    struct block_metadata* block;

    // Case 1: First allocation (initialize memory pool)
    if (global_head == NULL) {
        block = (struct block_metadata*)sbrk(size + META_SIZE);
        assert(block != (void*) -1); // Ensure allocation succeeded
        block->size = size;
        block->free = false;
        block->next = NULL;
        global_head = block;
        return (void*)(block + 1); // Return pointer just after metadata
    }

    // Case 2: Reuse a previously freed block if large enough
    block = global_head;
    while (block != NULL) {
        if (block->size >= size && block->free) {
            block->free = false;
            return (void*)(block + 1);
        }
        if (block->next == NULL) break; // Reached end of list
        block = block->next;
    }

    // Case 3: No suitable block found, allocate new block at the end
    struct block_metadata* newblock = (struct block_metadata*)sbrk(size + META_SIZE);
    assert(newblock != (void*) -1);
    newblock->size = size;
    newblock->free = false;
    newblock->next = NULL;
    block->next = newblock;
    return (void*)(newblock + 1);

}

// Get the metadata pointer corresponding to a user memory pointer
struct block_metadata* get_block_ptr(void* ptr) {
    return ((struct block_metadata*)ptr) - 1;
}

// Custom free implementation that marks the block as free
void free(void* ptr) {
    if (ptr == NULL) return;
    struct block_metadata* block_ptr = get_block_ptr(ptr);
    assert(block_ptr->free == false); // Check for double free
    block_ptr->free = true;
}

