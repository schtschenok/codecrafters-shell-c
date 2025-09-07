#include "base/string.h"

#include <assert.h>
int main(void) {
    arena_t arena = arena_make(1024);

    const c* some_string = "oooooaaoooaaaoo";
    str_t string_from_cstr = str_from_cstr(&arena, some_string);

    i64 context = -1;
    str_t token = { nullptr, 0, 0 };
    while (str_tokenize(&string_from_cstr, 'o', &context, &token)) {
        str_write(&token, stdout, true);
        printf("Context: %ld\n", context);
        printf("+++++\n");
    }

    return 0;
}