#pragma once

#include "arena.h"
#include "defines.h"

#include <stdio.h>

typedef struct {
    alloc_t alloc;
    u32 length;
    u32 capacity;
} str_t;

str_t str_from_alloc(alloc_t alloc);

str_t str_from_cstr(arena_t* arena, const c* source);

bool str_valid(const str_t* str);

bool str_copy(str_t* destination, const str_t* source);

bool str_eq(const str_t* str1, const str_t* str2);

bool str_eq_cstr(const str_t* str, const c* cstr);

bool str_find_next_after(const str_t* str, c character, i64* position);

bool str_tokenize(const str_t* str, c character, str_t* token, i64* context);

void str_write(const str_t* str, FILE* file, bool newline);

// List of shit to re-do:
// strtok, strcmp+, printf, strcat, strcpy, strlen
