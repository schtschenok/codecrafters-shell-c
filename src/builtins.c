#include "definitions.h"

const struct wstring_to_function_pair builtins[BUILTINS_LENGTH] = {{L"exit", builtin_exit},
                                                                   {L"echo", builtin_echo},
                                                                   {L"type", builtin_type}};

void get_program_path_from_name(wchar_t* input, wchar_t* output) {
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
                            swprintf(output, OUTPUT_BUFFER_SIZE, L"%s/%ls", dir, input);
                            return;
                        }
                    }
                }
            }
            dir = strtok_r(NULL, ":", &context);
        }
    }
    output[0] = L'\0';
}

void builtin_exit(wchar_t* input, wchar_t* output) {
    if (input == NULL) {
        exit(0);
    }

    const long int exit_code = wcstol(input, NULL, 10);
    exit(exit_code);
}

void builtin_echo(wchar_t* input, wchar_t* output) {
    if (!input) {
        swprintf(output, OUTPUT_BUFFER_SIZE, L"");
        return;
    }
    swprintf(output, OUTPUT_BUFFER_SIZE, L"%ls\n", input);
}

void builtin_type(wchar_t* input, wchar_t* output) {
    if (!input) {
        return;
    }

    for (int i = 0; i < BUILTINS_LENGTH; i++) {
        if (wcscmp(input, builtins[i].wstring) == 0) {
            swprintf(output, OUTPUT_BUFFER_SIZE, L"%ls is a shell builtin\n", input);
            return;
        }
    }

    get_program_path_from_name(input, reusable_wchar_buffer);
    if (reusable_wchar_buffer[0] != L'\0') {
        swprintf(output, OUTPUT_BUFFER_SIZE, L"%ls is %ls\n", input, reusable_wchar_buffer);
        return;
    }

    swprintf(output, OUTPUT_BUFFER_SIZE, L"%ls: not found\n", input);
}