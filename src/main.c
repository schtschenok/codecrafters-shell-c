#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#if defined(_WIN32) || defined(_WIN64)

#include <io.h>
#define strtok_r strtok_s
#define wcstok_r wcstok_s

#else

#include <locale.h>

#endif

#define INPUT_BUFFER_SIZE 1024
#define OUTPUT_BUFFER_SIZE 1024

static bool keep_running = true;

void s_print(const wchar_t* input) {
    wprintf(input);
    wprintf(L"\n");
}

void s_read(wchar_t* input, size_t* input_length) {
    fgetws(input, INPUT_BUFFER_SIZE, stdin);

    const size_t input_buffer_end = wcscspn(input, L"\r\n");
    input[input_buffer_end] = 0;
}

void s_eval(wchar_t* input, const size_t input_length, wchar_t* output, const size_t output_size) {
    wchar_t* context;
    const wchar_t* token = wcstok_r(input, L" ", &context);

    if (wcscmp(token, L"exit") == 0) {
        token = wcstok_r(NULL, L" ", &context);

        if (token == NULL) {
            exit(0);
        }

        const long int exit_code = wcstol(token, NULL, 10);
        exit(exit_code);
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

    size_t input_length = 0;

    while (keep_running) {
        wprintf(L"$ ");
        s_read(input_buffer, &input_length);
        s_eval(input_buffer, input_length, output_buffer, OUTPUT_BUFFER_SIZE);
        s_print(output_buffer);
    }
}
