#ifndef BUDDY_SIZE_H
#define BUDDY_SIZE_H

#include <stddef.h>

#ifndef MIN_ALLOC_SIZE_BITS
#define MIN_ALLOC_SIZE_BITS 5
#endif

#ifndef MAX_ALLOC_SIZE_BITS
#define MAX_ALLOC_SIZE_BITS 30
#endif

#define MIN_ALLOC_SIZE ((size_t)1ULL << MIN_ALLOC_SIZE_BITS)
#define MAX_ALLOC_SIZE ((size_t)1ULL << MAX_ALLOC_SIZE_BITS)

#endif
