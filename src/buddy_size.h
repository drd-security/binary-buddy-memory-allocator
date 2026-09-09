#ifndef BUDDY_SIZE_H
#define BUDDY_SIZE_H

#ifndef MIN_ALLOC_SIZE_BITS
#define MIN_ALLOC_SIZE_BITS 5
#endif

#define MIN_ALLOC_SIZE ((size_t)1 << MIN_ALLOC_SIZE_BITS)

#endif
