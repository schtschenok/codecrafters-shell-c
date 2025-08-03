#include "arena.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

// TODO: Return NULL for zero allocations or failed mmap/munmap

bool arena_valid(const arena_t* arena) {
    if (!arena || !arena->start || !arena->capacity) {
        return false;
    }
    return true;
}

bool alloc_valid(const alloc_t* alloc) {
    if (!alloc || !arena_valid(alloc->arena) || alloc->position + alloc->size > alloc->arena->capacity) {
        return false;
    }
    return true;
}

size_t align_size(const size_t size, const size_t alignment) {
    return (size + (alignment - 1)) & ~(alignment - 1);
}

arena_t arena_make(const size_t size) {
    assert(size > 0);

    const size_t aligned_size = align_size(size, getpagesize());

    char* arena_start = mmap(NULL,
                             aligned_size,
                             PROT_READ | PROT_WRITE,
                             MAP_PRIVATE | MAP_ANONYMOUS,
                             -1,
                             0);
    if (arena_start == MAP_FAILED) {
        perror("Unable to make an arena - mmap failed");
        exit(1); // TODO: Maybe something better could be done here?
    }

    const arena_t arena = {
        .start = arena_start,
        .capacity = aligned_size,
        .position = 0
    };
    return arena;
}

void arena_grow(arena_t* arena, const size_t size) {
    assert(arena_valid(arena));

    size_t new_capacity = arena->capacity;
    while (new_capacity - arena->position < size) {
        new_capacity = (size_t)(1.5f * (float)new_capacity);
    }

    new_capacity = align_size(new_capacity, getpagesize());

    void* new_memory_block = mmap(NULL,
                                  new_capacity,
                                  PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_ANONYMOUS,
                                  -1,
                                  0);
    if (new_memory_block == MAP_FAILED) {
        perror("Unable to grow an arena - mmap failed");
        exit(1); // TODO: Maybe something better could be done here?
    }

    memcpy(new_memory_block, arena->start, arena->position);
    if (munmap(arena->start, arena->capacity) != 0) {
        perror("Unable to grow an arena - munmap failed");
        exit(1); // TODO: Maybe something better could be done here?
    }
    arena->start = new_memory_block;
    arena->capacity = new_capacity;
}

alloc_t arena_alloc(arena_t* arena, const size_t size) {
    assert(arena_valid(arena));
    assert(size > 0);

    const size_t aligned_size = align_size(size, DEFAULT_ALIGNMENT);

    if (arena->capacity - arena->position < aligned_size) {
        arena_grow(arena, aligned_size);
    }

    const alloc_t alloc = {
        .arena = arena,
        .position = arena->position,
        .size = aligned_size
    };
    arena->position += aligned_size;
    return alloc;
}

void* arena_get_ptr(const alloc_t* alloc) {
    assert(alloc_valid(alloc));

    return (void*)((uintptr_t)alloc->arena->start + alloc->position);
}

void arena_clear(arena_t* arena) {
    assert(arena_valid(arena));

    arena->position = 0;
}

void arena_delete(arena_t* arena) {
    assert(arena_valid(arena));

    if (munmap(arena->start, arena->capacity) != 0) {
        perror("Unable to delete an arena - munmap failed");
        exit(1);
    }

    arena->start = NULL;
    arena->capacity = 0;
    arena->position = 0;

    arena = NULL;
}
