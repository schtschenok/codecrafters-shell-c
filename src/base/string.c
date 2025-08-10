#include "string.h"
#include "arena.h"
#include "defines.h"
#include <assert.h>
#include <string.h>

bool str_valid(const str_t* str) {
    if (!str || !str->start || str->capacity < str->length) {
        return false;
    }
    return true;
}

str_t str_from_size(arena_t* arena, const size_t length) {
    assert(arena_valid(arena));
    const void* ptr = arena_alloc(arena, length);
    if (ptr) {
        return (str_t){
            .start = (c*)ptr,
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

str_t str_from_cstr(arena_t* arena, const c* cstr) { // TODO: Think about allocating +1 byte for null terminator
    assert(arena_valid(arena));
    assert(cstr);

    const size_t source_string_length = strlen(cstr); // Needs to be changed if "c" isn't "char"
    const void* ptr = arena_alloc(arena, source_string_length);

    memcpy((c*)ptr, cstr, source_string_length); // TODO: Use memmove

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

    memcpy(destination->start, source->start, source->length); // TODO: Use memmove
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
    assert(*position < str->length - 1); // TODO: Added "- 1", NEEDS TESTING
    assert(*position >= -1);

    if (*position >= str->length) {
        return false;
    }

    while (*position < (i64)str->length) {
        (*position)++;
        if (str->start[*position] == character) {
            while (*position < (i64)str->length && str->start[*position] == character) {
                (*position)++;
            }
            return true;
        }
    }
    return false;
}

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
bool str_tokenize(const str_t* str, const c separator, i64* context, str_t* token) {
    assert(str_valid(str));
    assert(context);
    // assert(str_valid(token));  // Not needed as it's filled as a slice and don't need to be valid the first time
    assert(*context >= -1);

    if (*context >= str->length) {
        return false;
    }

    i64 start_position = *context + 1;
    bool seen_non_sep = false;
    i64 first_sep_found = -1;
    i64 last_sep_found = -1;
    for (i64 position = start_position; position < str->length; position++) {
        if (str->start[position] == separator) {
            if (!seen_non_sep) {
                start_position++;
                continue;
            }
            if (first_sep_found == -1) {
                first_sep_found = position;
            }
            last_sep_found = position;
        } else {
            seen_non_sep = true;
            if (last_sep_found != -1) {
                break;
            }
            if (position == str->length - 1) {
                first_sep_found = str->length;
                last_sep_found = str->length;
                break;
            }
        }
    }

    if (first_sep_found == -1) {
        return false;
    }

    token->start = str->start + start_position;
    token->length = first_sep_found - start_position;
    token->capacity = token->length;

    *context = last_sep_found;

    return true;
}

void str_write(const str_t* str, FILE* file, const bool newline) {
    assert(str_valid(str));

    fwrite(str->start, sizeof(c), str->length, file);

    if (newline) {
        fwrite("\n", 1, 1, file);
    }
}