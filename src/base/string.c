#include "string.h"
#include "arena.h"
#include "defines.h"
#include <assert.h>
#include <string.h>

str_t str_from_alloc(const alloc_t alloc) {
    return (str_t){
        .alloc = alloc,
        .capacity = alloc.size
    };
}

str_t str_from_cstr(arena_t* arena, const c* source) {
    assert(arena && arena->start); // Add arena_valid() function
    assert(source);

    const size_t source_string_length = strlen(source); // Needs to be changed if "c" isn't "char"
    const alloc_t alloc = arena_alloc(arena, source_string_length * sizeof(c));

    memcpy(arena_get_ptr(&alloc), source, source_string_length);

    const str_t string = {
        .alloc = alloc,
        .capacity = alloc.size,
        .length = source_string_length
    };

    return string;
}

bool str_valid(const str_t* str) {
    if (!str || !str->alloc.arena || !str->alloc.arena->start || str->capacity < str->length) {
        return false;
    }
    return true;
}

bool str_copy(str_t* destination, const str_t* source) {
    assert(str_valid(source));
    assert(str_valid(destination));

    if (destination->capacity < source->length) {
        return false;
    }

    memcpy(arena_get_ptr(&destination->alloc), arena_get_ptr(&source->alloc), source->length);
    destination->length = source->length;
    return true;
}

bool str_eq(const str_t* str1, const str_t* str2) {
    assert(str_valid(str1) && str_valid(str2));

    if (str1->length != str2->length) {
        return false;
    }

    if (str1->length == 0) {
        return true;
    }

    const c* ptr1 = arena_get_ptr(&str1->alloc);
    const c* ptr2 = arena_get_ptr(&str2->alloc);

    return memcmp(ptr1, ptr2, str1->length) == 0;
}

bool str_eq_cstr(const str_t* str, const c* cstr) {
    assert(str_valid(str));
    assert(cstr);

    if (str->length != strlen(cstr)) {
        return false;
    }

    if (str->length == 0) {
        return true;
    }

    const c* ptr1 = arena_get_ptr(&str->alloc);
    return memcmp(ptr1, cstr, str->length) == 0;
}

// Position should be initialized as -1
bool str_find_next_after(const str_t* str, const c character, i64* position) {
    assert(position);
    assert(str_valid(str));
    assert(*position < str->length);
    assert(*position >= -1);

    const c* base_ptr = arena_get_ptr(&str->alloc);

    while (*position < (i64)str->length) {
        (*position)++;
        if (base_ptr[*position] == character) {
            return true;
        }
    }
    return false;
}

// Context should be initialized as -1
bool str_tokenize(const str_t* str, const c character, str_t* token, i64* context) {
    assert(str_valid(str));
    assert(*context >= -1);

    const intptr_t start_position = *context + 1;

    if (start_position >= str->length) {
        return false;
    }

    alloc_t context_alloc = str->alloc;
    context_alloc.position = str->alloc.position + start_position;
    context_alloc.size = str->alloc.size - start_position;

    const str_t context_str = {
        .alloc = context_alloc,
        .length = str->length - start_position,
        .capacity = str->capacity - start_position
    };
    i64 position = -1;
    if (str_find_next_after(&context_str, character, &position)) {
        const str_t result = {
            .alloc = context_alloc,
            .length = position,
            .capacity = context_str.capacity
        };
        *context = start_position + position;
        *token = result;
        return true;
    }
    const str_t result = {
        .alloc = context_alloc,
        .length = str->length - start_position,
        .capacity = context_str.capacity
    };
    *context = str->length;
    *token = result;
    return true;
}

void str_write(const str_t* str, FILE* file, const bool newline) {
    assert(str_valid(str));

    fwrite(arena_get_ptr(&str->alloc), sizeof(c), str->length, file);

    if (newline) {
        fwrite("\n", sizeof(c), 1, file);
    }
}