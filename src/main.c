#include "base/arena.h"
#include "base/defines.h"
#include "base/string.h"
#include "builtins.h"
#include "definitions.h"

#include <locale.h>
#include <string.h>

// char reusable_char_buffer[REUSABLE_CHAR_BUFFER_SIZE];
// wchar_t reusable_wchar_buffer[REUSABLE_WCHAR_BUFFER_SIZE];

const char* syspath;

bool keep_running = true;

void s_print(const str_t* input) {
    str_write(input, stdout, true);
}

void s_read(arena_t* arena, str_t* output) {
    c input_buffer[INPUT_BUFFER_SIZE]; // TODO: Allocate depending on actual input size instead
    fgets(input_buffer, sizeof(input_buffer), stdin);
    *output = str_from_cstr(arena, input_buffer);
}

void s_eval(arena_t* command_arena, const str_t* input_str, str_t* output_str) {
    i64 context = -1;
    const alloc_t alloc = arena_alloc(command_arena, input_str->length);
    str_t token = str_from_alloc(alloc);

    const bool not_empty = str_tokenize(input_str, ' ', &token, &context);

    if (!not_empty || token.length == 0) {
        return;
    }

    for (int i = 0; i < BUILTINS_LENGTH; i++) {
        if (str_eq_cstr(&token, builtins[i].string)) {
            builtins[i].function(input_str, output_str);
            return;
        }
    }

    // wchar_t* context = NULL;
    // const wchar_t* token = wcstok(input, L" ", &context);
    //
    // if (token) {
    //     for (int i = 0; i < BUILTINS_LENGTH; i++) {
    //         if (wcscmp(token, builtins[i].wstring) == 0) {
    //             builtins[i].function(context, output);
    //             return;
    //         }
    //     }
    // }
    //
    // get_program_path_from_name(input, reusable_wchar_buffer);
    // if (reusable_wchar_buffer[0] != L'\0') {
    //     wcstombs(reusable_char_buffer, input, REUSABLE_CHAR_BUFFER_SIZE);
    //     if (context && context[0] != L'\0') {
    //         swprintf(reusable_wchar_buffer, REUSABLE_WCHAR_BUFFER_SIZE, L"%s %ls", reusable_char_buffer, context);
    //         wcstombs(reusable_char_buffer, reusable_wchar_buffer, REUSABLE_CHAR_BUFFER_SIZE);
    //     }
    //     system(reusable_char_buffer);
    //     return;
    // }
    //
    // swprintf(output, output_size, L"%ls: command not found\n", input);
}

int main(int argc, char* argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    arena_t command_arena = arena_make(4096);

    syspath = getenv("PATH");

    // ReSharper disable once CppDFALoopConditionNotUpdated
    while (keep_running) {
        arena_clear(&command_arena);
        printf("$ ");
        str_t input_str;
        str_t output_str;
        s_read(&command_arena, &input_str);
        s_eval(&command_arena, &input_str, &output_str);
        // s_print(&output_str);

        // str_write(&input_str, stdout, false);

        // i64 context = -1;
        // i64 position = -1;
        // while (str_find_next_after(&input_str, ' ', &position)) {
        //     str_write(&input_str, stdout, false);
        //     printf("%ld\n", position);
        // }

        // str_t token;
        //
        // bool more_tokens = true;
        // while (str_tokenize(&input_str, ' ', &token, &context)) {
        //     str_write(&token, stdout, true);
        // }

        // s_eval(input_buffer, output_buffer, OUTPUT_BUFFER_SIZE);
        // s_print(output_buffer);
    }
}
