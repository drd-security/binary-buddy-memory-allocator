#ifndef BINARY_BUDDY_H
#define BINARY_BUDDY_H

#include <stddef.h>
#include  <stdint.h>
#include "buddy_size.h"

// ------------------- START PROTECTED CODE -------------------

/** 
 * Initialize the buddy allocator with a memory region of given size.
 * Returns a pointer to the base of the memory region on success, NULL on failure.
 * @param size The total size of the memory region to manage. Must be a power of 2 and >= MIN_ALLOC_SIZE.
 */
void *init_buddy(size_t size);

/** 
 * Release the memory region used by the buddy allocator.
 * @return 0 on success, -1 on failure.
 */
int free_buddy();

/**
 * Allocate a memory chunk of given size.
 * Returns a pointer to the allocated chunk, NULL on failure.
 * @param size The size of the memory chunk to allocate. Must be a power of 2 and >= MIN_ALLOC_SIZE.
 */
void *balloc(size_t size);

/** 
 * Deallocate a previously allocated memory chunk.
 * @param ptr A pointer to the memory chunk to deallocate.
 */
void bfree(void *ptr);

/** 
 * Get the total used space in bytes in the allocator.
 * @return The total used space in bytes.
 */
size_t get_used_space();

typedef struct
{
    // The 3 fields below are protected
    const void *base;  // Base address of the memory region
    size_t used_space; // Total used space in the allocator
    size_t total_size; // Total size of the memory region

    // ------------------- END PROTECTED CODE -------------------

    size_t* tree; // Array representing the binary tree of free space
    size_t NUMBER_OF_LEAF_NODES; // Number of leaf nodes (total_size / M)

} Allocator; // Cannot modify the name of this struct, but you can add fields to it

// You can add code/struct here if needed

#endif // BINARY_BUDDY_H
