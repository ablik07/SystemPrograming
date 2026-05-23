#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif

// 1. Копирование с использованием библиотеки C
void copy_c(const char* src, const char* dst) {
    FILE* in = fopen(src, "rb");
    FILE* out = fopen(dst, "wb");

    if (!in || !out) {
        printf("Ошибка открытия файлов (C)\n");
        if (in) fclose(in);
        if (out) fclose(out);
        return;
    }

    char buffer[4096];
    size_t bytes;

    while ((bytes = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        fwrite(buffer, 1, bytes, out);
    }

    fclose(in);
    fclose(out);
    printf("Копирование (C) завершено\n");
}

// 2. Копирование с использованием Windows API
void copy_winapi(const char* src, const char* dst) {
    HANDLE hSrc = CreateFileA(src, GENERIC_READ, 0, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSrc == INVALID_HANDLE_VALUE) {
        printf("Ошибка открытия исходного файла (WINAPI)\n");
        return;
    }

    HANDLE hDst = CreateFileA(dst, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (hDst == INVALID_HANDLE_VALUE) {
        printf("Ошибка создания файла назначения (WINAPI)\n");
        CloseHandle(hSrc);
        return;
    }

    char buffer[4096];
    DWORD bytesRead, bytesWritten;

    while (ReadFile(hSrc, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0) {
        WriteFile(hDst, buffer, bytesRead, &bytesWritten, NULL);
    }

    CloseHandle(hSrc);
    CloseHandle(hDst);
    printf("Копирование (WINAPI) завершено\n");
}

