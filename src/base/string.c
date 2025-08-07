#include "string.h"
#include "arena.h"
#include "defines.h"
#include <assert.h>
#include <string.h>

bool str_valid(const str_t* str) {
    if (!str || !str->start || !str->capacity || str->capacity < str->length) {
        return false;
    }
    return true;
}

str_t str_from_size(arena_t* arena, const size_t length) {
    assert(arena_valid(arena));
    const void* ptr = arena_alloc(arena, length);
    if (ptr) {
        return (str_t){
            .start = (c*)arena_alloc(arena, length),
            .length = length,
            .capacity = length
        };
    }
    return (str_t){
        .start = NULL,
        .length = 0,
        .capacity = 0
    };
}

str_t str_from_cstr(arena_t* arena, const c* source) {
    assert(arena_valid(arena)); // Add arena_valid() function
    assert(source);

    const size_t source_string_length = strlen(source); // Needs to be changed if "c" isn't "char"
    const void* ptr = arena_alloc(arena, source_string_length);

    memcpy((c*)ptr, source, source_string_length);

    const str_t string = {
        .start = (c*)ptr,
        .capacity = source_string_length,
        .length = source_string_length
    };

    return string;
}

bool str_copy(str_t* destination, const str_t* source) {
    assert(str_valid(source));
    assert(str_valid(destination));

    if (destination->capacity < source->length) {
        return false;
    }

    memcpy(destination->start, source->start, source->length);
    destination->length = source->length;
    return true;
}

bool str_eq(const str_t* str1, const str_t* str2) {
    assert(str_valid(str1));
    assert(str_valid(str2));

    if (str1->length != str2->length) {
        return false;
    }

    if (str1->length == 0) {
        return true;
    }

    return memcmp(str1->start, str2->start, str1->length) == 0;
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

    return memcmp(str->start, cstr, str->length) == 0;
}

// Position should be initialized as -1 to start from the beginning
bool str_find_next_after(const str_t* str, const c character, i64* position) {
    assert(str_valid(str));
    assert(position);
    assert(*position < str->length);
    assert(*position >= -1);

    if (*position >= str->length) {
        return false;
    }

    while (*position < (i64)str->length) {
        (*position)++;
        if (str->start[*position] == character) {
            while (*position < (i64)str->length && str->start[*position] == character) { // TODO: FIX TOKENIZER!!!
                (*position)++;
            }
            return true;
        }
    }
    return false;
}

// TODO: FIX THIS SHIT

str_t str_slice(const str_t* source, const size_t start, const size_t end) {
    assert(str_valid(source));
    assert(end >= start);
    assert(start < source->length);
    assert(end < source->length);

    return (str_t){
        .start = source->start + start,
        .length = end - start,
        .capacity = source->capacity - start
    };
}

// Context should be initialized as -1 to start from the beginning
bool str_tokenize(const str_t* str, const c character, i64* context, str_t* token) {
    assert(str_valid(str));
    assert(context);
    // assert(str_valid(token));  // Not needed as it's filled as a slice
    assert(*context >= -1);

    if (*context >= str->length) {
        return false;
    }

    i64 start_position = *context + 1;
    i64 last_found = -1;
    i64 first_found = -1;
    for (i64 position = start_position; position < str->length; position++) {
        if (str->start[position] == character) {
            if (first_found == -1) {
                first_found = position;
            }
            last_found = position;
        } else {
            if (last_found != -1) {
                break;
            }
        }
    }

    if (first_found == -1) {
        return false;
    }

    token->start = str->start + start_position;
    token->length = first_found - start_position;
    token->capacity = str->capacity - start_position;

    *context = last_found; // TODO: IT'S NOT RETURNING THE LAST TOKEN!!!

    return true;

    // return token when fitst found and return context when last found !!!!!!
    // i64 position = *context;
    // i64 last_found = -1;
    // bool go_next = true;
    // while (go_next) {
    //     position++;
    //     if (str->start[position] == character) {
    //         go_next = true;
    //     } else {
    //         go_next = true;
    //     }
    // }
    //
    // context_alloc.position = str->alloc.position + start_position;
    // context_alloc.size = str->alloc.size - start_position;
    //
    // const str_t context_str = {
    //     .alloc = context_alloc,
    //     .length = str->length - start_position,
    //     .capacity = str->capacity - start_position
    // };
    // i64 position = -1;
    // if (str_find_next_after(&context_str, character, &position)) {
    //     const str_t result = {
    //         .alloc = context_alloc,
    //         .length = position,
    //         .capacity = context_str.capacity
    //     };
    //     *context = start_position + position;
    //     *token = result;
    //     return true;
    // }
    // const str_t result = {
    //     .alloc = context_alloc,
    //     .length = str->length - start_position,
    //     .capacity = context_str.capacity
    // };
    // *context = str->length;
    // *token = result;
    // return true;
}

void str_write(const str_t* str, FILE* file, const bool newline) {
    assert(str_valid(str));

    fwrite(str->start, sizeof(c), str->length, file);

    if (newline) {
        fwrite("\n", 1, 1, file);
    }
}