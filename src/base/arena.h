#pragma once

#include <stdlib.h>

#define DEFAULT_ALIGNMENT 8

typedef struct {
    void* start;
    size_t capacity;
    size_t position;
} arena_t;

typedef struct {
    arena_t* arena;
    size_t position;
    size_t size;
} alloc_t;

bool arena_valid(const arena_t* arena);

bool alloc_valid(const alloc_t* alloc);

size_t align_size(size_t size, size_t alignment);

arena_t arena_make(size_t size);

alloc_t arena_alloc(arena_t* arena, size_t size);

void* arena_get_ptr(const alloc_t* alloc);

void arena_clear(arena_t* arena);

void arena_delete(arena_t* arena);
