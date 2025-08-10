#pragma once

#include <stdint.h>
#include <stdlib.h>

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
