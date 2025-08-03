#pragma once
#include "base/string.h"

#define BUILTINS_LENGTH 3

struct wstring_to_function_pair {
    c* string;
    void (*function)(const str_t*, str_t*);
};

extern const struct wstring_to_function_pair builtins[BUILTINS_LENGTH];

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
