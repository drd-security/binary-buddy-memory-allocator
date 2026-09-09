# Binary Buddy Memory Allocator

A user-space implementation of a **binary buddy allocator** in C, managing a fixed memory region obtained with `mmap` and supporting logarithmic-time allocation/deallocation through a compact tree of maximum-free-block sizes.

## Key ideas

- Memory is partitioned into power-of-two blocks.
- Allocation selects the **lowest-address** block that can satisfy the request.
- Requests are rounded up to the smallest supported power-of-two block.
- A binary tree records the largest free block available in each subtree.
- Freeing a block updates ancestors and naturally coalesces free buddies.
- `used_space` tracks current allocated capacity.

## Public API

```c
void  *init_buddy(size_t size);
void  *balloc(size_t size);
void   bfree(void *ptr);
int    free_buddy(void);
size_t get_used_space(void);
```

## Data structure

The allocator stores one array-backed binary tree. A leaf represents one minimum-size block. Every internal node stores either the complete free size of its subtree (when both buddies are free) or the maximum free block available in one of its children.

This lets `balloc` descend from the root to the first suitable child and lets `bfree` recompute only the ancestor path, giving `O(log n)` tree operations.

## Build

```bash
make
```

The included `buddy_size.h` is a small **portfolio compatibility header**, not the original university skeleton file. Its defaults can be overridden at compile time, for example:

```bash
make CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -O2 -DMIN_ALLOC_SIZE_BITS=6'
```

## Academic context and contribution

Two-person operating-systems project. I completed the majority of the implementation; my teammate also contributed to the solution. The exercise focused on allocator correctness, fragmentation behavior, pointer validation, memory management, and performance relative to general-purpose allocation.

## Notes

The original course harness and protected skeleton/support files are not redistributed. This portfolio copy includes only the submitted allocator implementation plus a small replacement configuration header so the core source can be compiled independently.

## Publication status

See [NOTICE.md](NOTICE.md).
