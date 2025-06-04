#define _CRT_SECURE_NO_WARNINGS 1

#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#if defined(_WIN32) || defined(_WIN64)

#include <io.h>

#else

#include <locale.h>

#endif

#define INPUT_BUFFER_SIZE 1024
#define OUTPUT_BUFFER_SIZE 1024
#define SYSPATH_MAX_SIZE 4096

static const char* syspath;

#define BUILTIN_DEFINE(name) void builtin_##name(wchar_t* input, wchar_t* output)

static bool keep_running = true;

struct wstring_to_function_pair {
    wchar_t* wstring;
    void (*function)(wchar_t*, wchar_t*);
};

#define BUILTINS_LENGTH 3

BUILTIN_DEFINE(exit);
BUILTIN_DEFINE(echo);
BUILTIN_DEFINE(type);

static const struct wstring_to_function_pair builtins[BUILTINS_LENGTH] = {{L"exit", builtin_exit},
                                                                          {L"echo", builtin_echo},
                                                                          {L"type", builtin_type}};

void builtin_exit(wchar_t* input, wchar_t* output) {
    if (input == NULL) {
        exit(0);
    }

    const long int exit_code = wcstol(input, NULL, 10);
    exit(exit_code);
}

void builtin_echo(wchar_t* input, wchar_t* output) {
    if (!input) {
        input = L"";
    }
    swprintf(output, OUTPUT_BUFFER_SIZE, L"%ls", input);
}

void builtin_type(wchar_t* input, wchar_t* output) {
    for (int i = 0; i < BUILTINS_LENGTH; i++) {
        if (wcscmp(input, builtins[i].wstring) == 0) {
            swprintf(output, OUTPUT_BUFFER_SIZE, L"%ls is a shell builtin", input);
            return;
        }
    }

    if (syspath) {
        char path[SYSPATH_MAX_SIZE];
        sprintf(path, "%s", syspath);

        char* context = NULL;
        const char* dir = strtok_r(path, ":", &context);

        struct dirent* de;

        while (dir != NULL) {
            DIR* dr = opendir(dir);
            if (dr) {
                while ((de = readdir(dr)) != NULL) {
                    if (de->d_type == DT_REG) {
                        mbstowcs(output, de->d_name, OUTPUT_BUFFER_SIZE);
                        if (wcscmp(input, output) == 0) {
                            swprintf(output, OUTPUT_BUFFER_SIZE, L"%ls is %s/%ls", input, dir, input);
                            return;
                        }
                    }
                }
            }
            dir = strtok_r(NULL, ":", &context);
        }
    }

    swprintf(output, OUTPUT_BUFFER_SIZE, L"%ls: not found", input);
}

void s_print(const wchar_t* input) {
    fputws(input, stdout);
    fputws(L"\n", stdout);
}

void s_read(wchar_t* input) {
    fgetws(input, INPUT_BUFFER_SIZE, stdin);

    const size_t input_buffer_end = wcscspn(input, L"\r\n");
    input[input_buffer_end] = 0;
}

void s_eval(wchar_t* input, wchar_t* output, const size_t output_size) {
    wchar_t* context = NULL;
    const wchar_t* token = wcstok(input, L" ", &context);

    if (token) {
        for (int i = 0; i < BUILTINS_LENGTH; i++) {
            if (wcscmp(token, builtins[i].wstring) == 0) {
                builtins[i].function(context, output);
                return;
            }
        }
    }

    swprintf(output, output_size, L"%ls: command not found", input);
}

int main(int argc, char* argv[]) {

#if defined(_WIN32) || defined(_WIN64)
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stderr), _O_U16TEXT);
#else
    setlocale(LC_CTYPE, "");
#endif

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    wchar_t input_buffer[INPUT_BUFFER_SIZE];
    wchar_t output_buffer[OUTPUT_BUFFER_SIZE];

    syspath = getenv("PATH");

    while (keep_running) {
        wprintf(L"$ ");
        s_read(input_buffer);
        s_eval(input_buffer, output_buffer, OUTPUT_BUFFER_SIZE);
        s_print(output_buffer);
    }
}
