#define ARENA_IMPLEMENTATION
#include "base/arena.h"
#define STRING_IMPLEMENTATION
#include "base/string.h"

#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#define BUILTINS_LENGTH 3

struct string_to_function_pair { // TODO: Can this be done better?
    char* string;
    void (*function)(arena_t* arena, const str_t, str_t*);
};

const struct string_to_function_pair builtins[BUILTINS_LENGTH];

#define BUILTIN_DEFINE(name) void builtin_##name(arena_t* arena, const str_t input, str_t* output)

BUILTIN_DEFINE(exit);
BUILTIN_DEFINE(echo);
BUILTIN_DEFINE(type);

const struct string_to_function_pair builtins[BUILTINS_LENGTH] = { { "exit", builtin_exit },
                                                                   { "echo", builtin_echo },
                                                                   { "type", builtin_type } };

const char* syspath;

void get_program_path_from_name(arena_t* arena, const str_t input, str_t* output) {
    if (syspath) {
        str_t path = str_from_cstr(arena, syspath);

        i64 context = -1;
        str_t dir = str_from_size(arena, path.length);

        bool not_empty = str_tokenize(path, ':', &context, &dir);

        if (!not_empty) {
            return;
        }
        struct dirent* de;

        while (not_empty) {
            char* dir_cstr = str_to_cstr(arena, dir);
            DIR* dr = opendir(dir_cstr);
            if (dr) {
                while ((de = readdir(dr)) != NULL) {
                    if (de->d_type == DT_REG) {
                        if (str_eq_cstr(input, de->d_name)) {
                            *output = str_from_size(arena, dir.length + input.length + 1);
                            str_copy(output, dir);
                            output->start[dir.length] = '/';
                            str_t program_name = str_slice(*output, dir.length + 1, output->capacity);
                            str_copy(&program_name, input);
                            output->length = output->capacity;
                            return;
                        }
                    }
                }
            }
            not_empty = str_tokenize(path, ':', &context, &dir);
        }
    }
}

void builtin_exit(arena_t* arena, const str_t input, str_t* output) {
    if (!str_valid(input) || input.length == 0) {
        exit(0);
    }
    // TODO: This is shitty, we need our own str_t -> int function
    char buf[64];
    if (input.length >= sizeof(buf)) {
        exit(0);
    }
    memcpy(buf, input.start, input.length);
    buf[input.length] = '\0';
    const int exit_code = (int)strtol(buf, NULL, 10);

    exit(exit_code);
}

void builtin_echo(arena_t* arena, const str_t input, str_t* output) {
    if (!str_valid(input) || input.length == 0) {
        return;
    }

    *output = input;
}

void builtin_type(arena_t* arena, const str_t input, str_t* output) {
    if (!str_valid(input) || input.length == 0) {
        return;
    }

    for (int i = 0; i < BUILTINS_LENGTH; i++) {
        if (str_eq_cstr(input, builtins[i].string)) {
            *output = str_from_size(arena, input.length + 19);
            str_copy(output, input);
            // TODO: Write str_append() or something
            memmove(output->start + input.length, " is a shell builtin", 19);
            output->length += 19;
            return;
        }
    }

    str_t program_name;
    get_program_path_from_name(arena, input, &program_name);
    if (str_valid(program_name) && program_name.length) {
        *output = str_from_size(arena, input.length + program_name.length + 4);
        str_copy(output, input);
        memmove(output->start + input.length, " is ", 4);
        str_t path_slice = str_slice(*output, input.length + 4, output->capacity);
        str_copy(&path_slice, program_name);
        output->length = output->capacity;
        return;
    }

    *output = str_from_size(arena, input.length + 11);
    str_copy(output, input);
    memmove(output->start + input.length, ": not found", 11);
    output->length += 11;
}

const char* syspath;

bool keep_running = true;

void s_print(const str_t input) {
    if (str_valid(input)) {
        str_write(input, stdout, true);
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
    memcpy(output->start, input_buffer, output->capacity);
    output->length = output->capacity;
}

void s_eval(arena_t* arena, const str_t input_str, str_t* output_str) {
    i64 context = -1;
    str_t token = str_from_size(arena, input_str.length);

    const bool not_empty = str_tokenize(input_str, ' ', &context, &token);

    // Make sure we're not passing a leading space if there's one
    i64 token_offset = context;
    if (context >= 0 && context < input_str.length) {
        token_offset = context + 1;
    }

    str_t args = {
        .start = input_str.start + token_offset,
        .length = input_str.length - token_offset,
        .capacity = input_str.capacity - token_offset
    };

    args = str_trim(args);

    if (!not_empty || token.length == 0) {
        return;
    }

    for (int i = 0; i < BUILTINS_LENGTH; i++) {
        if (str_eq_cstr(token, builtins[i].string)) {
            builtins[i].function(arena, args, output_str);
            return;
        }
    }

    str_t program_path;
    get_program_path_from_name(arena, token, &program_path);

    if (str_valid(program_path) && program_path.length) {
        const char* program_path_cstr = str_to_cstr(arena, program_path);
        system(program_path_cstr);
        return;
    }

    *output_str = str_from_size(arena, token.length + 19);
    str_copy(output_str, token);
    memmove(output_str->start + token.length, ": command not found", 19);
    output_str->length += 19;
}

void cleanup(int status, void* arg) {
    arena_clear((arena_t*)arg);
    arena_delete((arena_t*)arg);
}

int main(int argc, char* argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    arena_t command_arena = arena_make(1024UL * 1024);

    // IDK why?
    on_exit(cleanup, &command_arena);

    while (keep_running) {
        arena_clear(&command_arena);
        syspath = getenv("PATH");
        printf("$ ");
        str_t input_str = { NULL, 0, 0 };
        str_t output_str = { NULL, 0, 0 };
        s_read(&command_arena, &input_str);
        s_eval(&command_arena, input_str, &output_str);
        s_print(output_str);
    }
}
