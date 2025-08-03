#include "base/string.h"

#include <assert.h>
int main(void) {
    arena_t arena = arena_make(1024);

    const alloc_t alloc = arena_alloc(&arena, 256);
    str_t string_from_alloc = str_from_alloc(alloc);

    const c* some_string = "Hello dis is some awesome string";
    const str_t string_from_cstr = str_from_cstr(&arena, some_string);

    const bool string_from_alloc_valid = str_valid(&string_from_alloc);
    const bool string_from_cstr_valid = str_valid(&string_from_cstr);

    const bool copy_success = str_copy(&string_from_alloc, &string_from_cstr);
    const bool equal = str_eq(&string_from_alloc, &string_from_cstr);

    assert((string_from_alloc_valid + string_from_cstr_valid + copy_success + equal) == 4);

    i64 context = -1;

    // while (str_find_next_after(&string_from_cstr, ' ', &position)) {
    //     str_write(&string_from_cstr, stdout, true);
    //     printf("Position: %ld\n\n", position);
    // }

    while (str_tokenize(&string_from_cstr, ' ', &string_from_alloc, &context)) {
        str_write(&string_from_alloc, stdout, true);
        printf("Context: %ld\n", context);
    }

    return 0;
}