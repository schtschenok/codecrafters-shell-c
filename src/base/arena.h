#pragma once

#include "defines.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define DEFAULT_ALIGNMENT 8

typedef struct {
    uintptr_t start;
    size_t capacity;
    size_t position;
} arena_t;

size_t align_size(size_t size, size_t alignment);
bool arena_valid(const arena_t* arena);
arena_t arena_make(size_t size);
void* arena_alloc(arena_t* arena, size_t size);
void arena_clear(arena_t* arena);
void arena_delete(arena_t* arena);

static bool arena__is_power_of_two(size_t x) {
    return x != 0 && (x & (x - 1)) == 0;
}

/*
    Implementation section:

    Define ARENA_IMPLEMENTATION in exactly one C/C++ file BEFORE including this header:
        #define ARENA_IMPLEMENTATION
        #include "arena.h"

    Do NOT define ARENA_IMPLEMENTATION in more than one translation unit.
*/
#ifdef ARENA_IMPLEMENTATION

size_t align_size(const size_t size, const size_t alignment) {
    assert(arena__is_power_of_two(alignment)); // TODO: Needs testing
    return (size + (alignment - 1)) & ~(alignment - 1);
}

bool arena_valid(const arena_t* arena) {
    if (!arena || !arena->start || !arena->capacity) {
        return false;
    }
    return true;
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
        perror("Unable to create an arena - mmap failed");
        exit(1);
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
    if (arena_valid(arena)) {
        if (munmap((void*)arena->start, arena->capacity) != 0) {
            perror("Unable to delete an arena - munmap failed");
            exit(1);
        }
    }

    arena->start = 0;
    arena->capacity = 0;
    arena->position = 0;
}

#endif // ARENA_IMPLEMENTATION