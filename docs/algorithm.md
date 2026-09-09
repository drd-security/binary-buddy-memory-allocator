# Algorithm

## Allocation

1. Reject zero-sized or oversized requests.
2. Round the request up to the smallest supported power-of-two block.
3. Check the root value; if it is smaller than the target block, allocation fails.
4. Descend the tree, always preferring the left child when it can satisfy the request. This implements the required lowest-address policy.
5. Mark the selected node unavailable and update ancestor summaries.

## Free

1. Validate that the pointer belongs to the managed region and is aligned to the minimum block size.
2. Locate the corresponding leaf position and climb until the allocated node is found.
3. Mark that block free.
4. Recompute ancestors. If both child nodes represent fully free buddies, the parent becomes a fully free block of twice the size.

## Complexity

Tree traversal and ancestor updates are proportional to the tree height, so the structural work of allocation and deallocation is `O(log n)` in the number of minimum-sized blocks.
