#include "definitions.h"

char reusable_char_buffer[REUSABLE_CHAR_BUFFER_SIZE];
wchar_t reusable_wchar_buffer[REUSABLE_WCHAR_BUFFER_SIZE];

const char* syspath;

bool keep_running = true;

void s_print(const wchar_t* input) {
    fputws(input, stdout);
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

    get_program_path_from_name(input, reusable_wchar_buffer);
    if (reusable_wchar_buffer[0] != L'\0') {
        wcstombs(reusable_char_buffer, input, REUSABLE_CHAR_BUFFER_SIZE);
        if (context && context[0] != L'\0') {
            swprintf(reusable_wchar_buffer, REUSABLE_WCHAR_BUFFER_SIZE, L"%s %ls", reusable_char_buffer, context);
            wcstombs(reusable_char_buffer, reusable_wchar_buffer, REUSABLE_CHAR_BUFFER_SIZE);
        }
        system(reusable_char_buffer);
        return;
    }

    swprintf(output, output_size, L"%ls: command not found\n", input);
}

void clear_buffers(wchar_t* input, wchar_t* output) {
    input[0] = L'\0';
    output[0] = L'\0';
    reusable_char_buffer[0] = '\0';
    reusable_wchar_buffer[0] = L'\0';
}

int main(int argc, char* argv[]) {
    setlocale(LC_CTYPE, "");

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    wchar_t input_buffer[INPUT_BUFFER_SIZE];
    wchar_t output_buffer[OUTPUT_BUFFER_SIZE];

    syspath = getenv("PATH");

    while (keep_running) {
        clear_buffers(input_buffer, output_buffer);
        wprintf(L"$ ");
        s_read(input_buffer);
        s_eval(input_buffer, output_buffer, OUTPUT_BUFFER_SIZE);
        s_print(output_buffer);
    }
}
