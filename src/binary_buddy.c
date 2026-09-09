#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include "../include/binary_buddy.h"

/*
* Initalize the structures needed for the buddy allocator.
* Returns 0 on success, -1 on failure.
*/
static int init_structures(const void* memory_base, size_t size);

/*
* Free the buddy allocator's internal data structures.
*/
static void free_structures();

// ------------------- START PROTECTED CODE -------------------

Allocator a;

void* init_buddy(size_t size){
    void* memory_base = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (memory_base == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }
    
    if (init_structures(memory_base, size) != 0) { 
        munmap(memory_base, size);
        return NULL;
    }
    
    return memory_base;
}

int free_buddy() {
    free_structures();
    return munmap((void*)a.base, a.total_size);
}

size_t get_used_space(){
    return a.used_space;
}

// ------------------- END PROTECTED CODE -------------------

void* balloc(size_t size) {
    if (size == 0 || size > a.total_size) return NULL;
    
    // Calculate the target size (T), which must be a power of 2 >= max(size, M)
    size_t T = MIN_ALLOC_SIZE;
    if (size > MIN_ALLOC_SIZE) {
        // Use a native CPU instruction to round up to the next power of two in O(1).
        T = (size_t)1 << (64 - __builtin_clzll((unsigned long long)(size - 1)));
    }
    
    if (T > a.total_size) {
        return NULL;
    }
    
    // Check if the root has enough contiguous free space
    if (a.tree[1] < T) {
        return NULL; // Not enough space
    } 
    
    // Traverse the tree to find the appropriate block
    size_t i = 1;
    size_t current_size = a.total_size;
    size_t offset = 0;
    
    while (current_size > T) {
        current_size >>= 1;
        size_t left_child = i << 1;
        
        if (a.tree[left_child] >= T) {
            i = left_child;
        } else {
            i = left_child | 1;
            offset += current_size;
        }
    }
    
    a.tree[i] = 0; // 0 free space remaining in this subtree
    a.used_space += current_size;
    
    // Update the max free space available in all ancestors
    size_t parent = i >> 1;
    size_t node_size = current_size << 1;
    size_t child_size = current_size;
    
    while (parent > 0) {
        size_t left = a.tree[parent << 1];
        size_t right = a.tree[(parent << 1) | 1];
        size_t new_val;

        if (left == child_size && right == child_size) {
            new_val = node_size; // Both children completely free -> parent is completely free
        } else {
            new_val = (left > right) ? left : right; // Max of the two children
        }

        if (a.tree[parent] == new_val)
        {
            break; // No change in max free space, can stop updating ancestors
        }
        
        a.tree[parent] = new_val;
        child_size = node_size;
        parent >>= 1;
        node_size <<= 1;
    }
    
    return (void*)((char*)a.base + offset);
}

void bfree(void* ptr) {
    if (!ptr) return;
    
    // Validate pointer bounds
    if (ptr < a.base || (char*)ptr >= (char*)a.base + a.total_size) return;
    
    size_t offset = (char*)ptr - (char*)a.base;
    
    // The offset must be aligned with the minimum block size
    if ((offset & (MIN_ALLOC_SIZE - 1)) != 0) return;
    
    // Locate the corresponding leaf node in the tree
    size_t i = a.NUMBER_OF_LEAF_NODES + (offset / MIN_ALLOC_SIZE);
    size_t node_size = MIN_ALLOC_SIZE;
    
    // Climb up the tree to find exactly which node was allocated
    int found = 0;
    while (i > 0) {
        if (a.tree[i] == 0) {
            found = 1;
            break;
        }
        i >>= 1;
        node_size <<= 1;
    }
    
    // Prevent double-free or freeing an invalid pointer
    if (!found) return; 
    
    a.tree[i] = node_size; // The entire block is now free
    a.used_space -= node_size;
    
    // Update the ancestors (merging buddies logic is naturally handled here)
    size_t parent = i >> 1;
    size_t child_size = node_size;
    node_size <<= 1;
    
    while (parent > 0) {
        size_t left = a.tree[parent << 1];
        size_t right = a.tree[(parent << 1) | 1];
        size_t new_val;
        
        if (left == child_size && right == child_size) {
            new_val = node_size; // Buddies merged!
        } else {
            new_val = (left > right) ? left : right;
        }

        if (a.tree[parent] == new_val) {
            break; // No change in max free space, can stop updating ancestors
        }   

        a.tree[parent] = new_val;
        child_size = node_size;
        parent >>= 1;
        node_size <<= 1;
    }
}

static int init_structures(const void* memory_base, size_t size){
    a.base = memory_base;
    a.total_size = size;
    a.used_space = 0;

    if (size < MIN_ALLOC_SIZE) {
        return -1; // Total size must be at least the minimum block size
    }
    
    a.NUMBER_OF_LEAF_NODES = size / MIN_ALLOC_SIZE;
    
    // Allocate the binary tree structure
    // We use calloc/malloc for the metadata to avoid consuming the user mmap region
    a.tree = (size_t*)malloc(2 * a.NUMBER_OF_LEAF_NODES * sizeof(size_t));
    if (!a.tree) {
        return -1;
    }
    
    // Initialize the tree values bottom-up
    // Leaves
    for (size_t i = a.NUMBER_OF_LEAF_NODES; i < 2 * a.NUMBER_OF_LEAF_NODES; i++) {
        a.tree[i] = MIN_ALLOC_SIZE;
    }

    // Internal nodes
    for (size_t i = a.NUMBER_OF_LEAF_NODES - 1; i > 0; i--) {
        a.tree[i] = a.tree[i << 1] + a.tree[(i << 1) | 1];
    }
    
    return 0;
}

static void free_structures() {
    if (a.tree) {
        free(a.tree);
        a.tree = NULL;
    }
}