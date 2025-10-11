#define ARENA_IMPLEMENTATION
#include "base/arena.h"

#include <string.h>

int main(void) {
    arena_t arena = arena_make(1024UL * 1024 * 1024 * 8);
    char* allocation = arena_alloc(&arena, 1024UL * 1024 * 1024 * 4);
    memset(allocation + 1024UL * 1024 * 1024 * 2, 0, 1024UL * 1024 * 1024 * 2);
    // memset(allocation, 0, 1024UL * 1024 * 1024 * 4 + 8);
    arena_release(&arena);
    allocation = arena_alloc(&arena, 1024UL * 1024 * 1024 * 8);
    memset(allocation, 0, 1024UL * 1024 * 1024 * 8);
    arena_release(&arena);
}
