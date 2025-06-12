#pragma once

#define _CRT_SECURE_NO_WARNINGS 1

#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

#define INPUT_BUFFER_SIZE 4096
#define OUTPUT_BUFFER_SIZE 4096
#define REUSABLE_CHAR_BUFFER_SIZE 4096
#define REUSABLE_WCHAR_BUFFER_SIZE 4096
#define SYSPATH_MAX_SIZE 4096

extern char reusable_char_buffer[REUSABLE_CHAR_BUFFER_SIZE];
extern wchar_t reusable_wchar_buffer[REUSABLE_WCHAR_BUFFER_SIZE];

extern const char* syspath;

extern bool keep_running;

#define BUILTIN_DEFINE(name) void builtin_##name(wchar_t* input, wchar_t* output)

struct wstring_to_function_pair {
    wchar_t* wstring;
    void (*function)(wchar_t*, wchar_t*);
};

#define BUILTINS_LENGTH 3

BUILTIN_DEFINE(exit);
BUILTIN_DEFINE(echo);
BUILTIN_DEFINE(type);

extern const struct wstring_to_function_pair builtins[BUILTINS_LENGTH];

void get_program_path_from_name(wchar_t* input, wchar_t* output);