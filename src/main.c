#define ARENA_IMPLEMENTATION
#include "base/arena.h"
#include "base/defines.h"
#define STRING_IMPLEMENTATION
#include "base/string.h"
#include <string.h>

#define BUILTINS_LENGTH 3

struct string_to_function_pair { // TODO: Is this a good way to do LUTs?
    char* string;
    void (*function)(const str_t*, str_t*);
};

extern const struct string_to_function_pair builtins[BUILTINS_LENGTH];

// #define SYSPATH_MAX_SIZE 4096
//
#define BUILTIN_DEFINE(name) void builtin_##name(const str_t* input, str_t* output)
//
// const char* syspath;
//
//

void get_program_path_from_name(const str_t* input, str_t* output);

BUILTIN_DEFINE(exit);
BUILTIN_DEFINE(echo);
BUILTIN_DEFINE(type);
//

const struct string_to_function_pair builtins[BUILTINS_LENGTH] = { { "exit", builtin_exit },
                                                                   { "echo", builtin_echo },
                                                                   { "type", builtin_type } };

void get_program_path_from_name(const str_t* input, str_t* output) {
    // if (syspath) {
    //     char path[SYSPATH_MAX_SIZE];
    //     sprintf(path, "%s", syspath);
    //
    //     char* context = NULL;
    //     const char* dir = strtok_r(path, ":", &context);
    //
    //     struct dirent* de;
    //
    //     while (dir != NULL) {
    //         DIR* dr = opendir(dir);
    //         if (dr) {
    //             while ((de = readdir(dr)) != NULL) {
    //                 if (de->d_type == DT_REG) {
    //                     mbstowcs(output, de->d_name, OUTPUT_BUFFER_SIZE);
    //                     if (wcscmp(input, output) == 0) {
    //                         swprintf(output, OUTPUT_BUFFER_SIZE, L"%s/%ls", dir, input);
    //                         return;
    //                     }
    //                 }
    //             }
    //         }
    //         dir = strtok_r(NULL, ":", &context);
    //     }
    // }
    // output[0] = L'\0';
}

void builtin_exit(const str_t* input, str_t* output) {
    printf("YOO EXIT");
    // if (input == NULL) {
    //     exit(0);
    // }
    //
    // const long int exit_code = wcstol(input, NULL, 10);
    // exit(exit_code);
}

void builtin_echo(const str_t* input, str_t* output) {
    if (!str_valid(input) || input->length == 0) {
        return;
    }

    *output = *input;
}

void builtin_type(const str_t* input, str_t* output) {
    printf("YOO TYPE");
    // if (!input) {
    //     return;
    // }
    //
    // for (int i = 0; i < BUILTINS_LENGTH; i++) {
    //     if (wcscmp(input, builtins[i].wstring) == 0) {
    //         swprintf(output, OUTPUT_BUFFER_SIZE, L"%ls is a shell builtin\n", input);
    //         return;
    //     }
    // }
    //
    // get_program_path_from_name(input, reusable_wchar_buffer);
    // if (reusable_wchar_buffer[0] != L'\0') {
    //     swprintf(output, OUTPUT_BUFFER_SIZE, L"%ls is %ls\n", input, reusable_wchar_buffer);
    //     return;
    // }
    //
    // swprintf(output, OUTPUT_BUFFER_SIZE, L"%ls: not found\n", input);
}

const char* syspath;

bool keep_running = true;

void s_print(const str_t* input) {
    if (str_valid(input)) {
        str_write(input, stdout, false);
    }
}

void s_read(arena_t* arena, str_t* output) {
    char input_buffer[4096]; // TODO: Allocate depending on actual input size instead
    fgets(input_buffer, sizeof(input_buffer), stdin);
    const size_t input_str_len = strlen(input_buffer);
    size_t actual_input_len = input_str_len;
    if (input_str_len > 0 && input_buffer[input_str_len - 1] == '\n') {
        actual_input_len = input_str_len - 1;
    }
    *output = str_from_size(arena, actual_input_len);
    memcpy(output->start, input_buffer, output->length);
}

void s_eval(arena_t* arena, const str_t* input_str, str_t* output_str) {
    i64 context = -1;
    str_t token = str_from_size(arena, input_str->length);

    const bool not_empty = str_tokenize(input_str, ' ', &context, &token);

    i64 token_offset = 0;
    if (context >= 0) {
        token_offset = context + 1;
    }
    const str_t args = {
        .start = input_str->start + token_offset,
        .length = input_str->length - token_offset,
        .capacity = input_str->capacity - token_offset
    };

    if (!not_empty || token.length == 0) {
        return;
    }

    for (int i = 0; i < BUILTINS_LENGTH; i++) {
        if (str_eq_cstr(&token, builtins[i].string)) {
            builtins[i].function(&args, output_str);
            return;
        }
    }

    str_t program_path = str_from_size(arena, 4096 + sizeof(char));
    get_program_path_from_name(&token, &program_path);

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

    arena_t command_arena = arena_make(1024UL * 1024);

    // ReSharper disable once CppDFALoopConditionNotUpdated
    while (keep_running) {
        arena_clear(&command_arena);
        syspath = getenv("PATH");
        printf("$ ");
        str_t input_str = { NULL, 0, 0 };
        str_t output_str = { NULL, 0, 0 };
        s_read(&command_arena, &input_str);
        s_eval(&command_arena, &input_str, &output_str);
        s_print(&output_str);

        // TODO: echo is incredibly broken now if there's no args to it

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
