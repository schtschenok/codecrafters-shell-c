#include "arena.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

bool arena_valid(const arena_t* arena) {
    if (!arena || !arena->start || !arena->capacity) {
        return false;
    }
    return true;
}

size_t align_size(const size_t size, const size_t alignment) { // TODO: Can we check if it's a power of two comptime?
    return (size + (alignment - 1)) & ~(alignment - 1);
}

arena_t arena_make(const size_t size) {
    assert(size > 0);

    const size_t aligned_size = align_size(size, getpagesize());

    void* arena_start = mmap(NULL,
                             aligned_size,
                             PROT_READ | PROT_WRITE,
                             MAP_PRIVATE | MAP_ANONYMOUS,
                             -1,
                             0);
    if (arena_start == MAP_FAILED) {
        perror("Unable to make an arena - mmap failed");
        exit(1); // TODO: Return an invalid arena maybe?
    }

    const arena_t arena = {
        .start = (uintptr_t)arena_start,
        .capacity = aligned_size,
        .position = 0
    };
    return arena;
}

void* arena_alloc(arena_t* arena, const size_t size) {
    assert(arena_valid(arena));

    if (size == 0) {
        return NULL; // TODO: Think about returning a current position pointer
    }

    const size_t aligned_size = align_size(size, DEFAULT_ALIGNMENT);

    if (arena->capacity - arena->position < aligned_size) {
        return NULL;
    }

    void* ptr = (void*)(arena->start + arena->position);
    arena->position += aligned_size;
    return ptr;
}

void arena_clear(arena_t* arena) {
    assert(arena_valid(arena));

    arena->position = 0;
}

void arena_delete(arena_t* arena) {
    assert(arena_valid(arena)); // TODO: Do we need to assert for double-deletion?

    if (munmap((void*)arena->start, arena->capacity) != 0) {
        perror("Unable to delete an arena - munmap failed");
        exit(1);
    }

    arena->start = 0;
    arena->capacity = 0;
    arena->position = 0;

    arena = NULL; // TODO: Bullshit
}
