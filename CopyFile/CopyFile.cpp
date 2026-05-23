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