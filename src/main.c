#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <wchar.h>

#ifdef _WIN32
#include <io.h>
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

void s_read(wchar_t* input) {
    fgetws(input, INPUT_BUFFER_SIZE, stdin);

    const size_t input_buffer_end = wcscspn(input, L"\r\n");
    input[input_buffer_end] = 0;
}

void s_eval(const wchar_t* input, const size_t output_size, wchar_t* output) {
    swprintf(output, output_size, L"%ls: command not found", input);
}

int main(int argc, char* argv[]) {

#ifdef _WIN32
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stderr), _O_U16TEXT);
#else
    setlocale(LC_CTYPE, "");
#endif

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    wchar_t input_buffer[INPUT_BUFFER_SIZE];
    wchar_t output_buffer[OUTPUT_BUFFER_SIZE];

    while (keep_running) {
        wprintf(L"$ ");
        s_read(input_buffer);
        s_eval(input_buffer, OUTPUT_BUFFER_SIZE, output_buffer);
        s_print(output_buffer);
    }
}
