#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

int main()
{
    string inputFileName, outputFileName;

    cout << "Enter input file name (ASCII): ";
    cin >> inputFileName;
    cout << "Enter output file name (Unicode UTF-16): ";
    cin >> outputFileName;

    // 1. Открываем входной файл
    HANDLE hInputFile = CreateFileA(
        inputFileName.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hInputFile == INVALID_HANDLE_VALUE)
    {
        cerr << "Cannot open input file. Error: " << GetLastError() << endl;
        cin.get(); cin.get();
        return -1;
    }

    // 2. Узнаём размер входного файла
    DWORD inputSize = GetFileSize(hInputFile, NULL);
    if (inputSize == INVALID_FILE_SIZE)
    {
        cerr << "Cannot get file size. Error: " << GetLastError() << endl;
        CloseHandle(hInputFile);
        cin.get(); cin.get();
        return -1;
    }

    if (inputSize == 0)
    {
        cerr << "Input file is empty." << endl;
        CloseHandle(hInputFile);
        cin.get(); cin.get();
        return -1;
    }

    // 3. Отображаем входной файл в память
    HANDLE hInputMapping = CreateFileMapping(
        hInputFile,
        NULL,
        PAGE_READONLY,
        0,
        0,
        NULL
    );

    if (hInputMapping == NULL)
    {
        cerr << "Cannot create file mapping for input. Error: " << GetLastError() << endl;
        CloseHandle(hInputFile);
        cin.get(); cin.get();
        return -1;
    }

    LPCSTR pInputData = (LPCSTR)MapViewOfFile(
        hInputMapping,
        FILE_MAP_READ,
        0,
        0,
        0
    );

    if (pInputData == NULL)
    {
        cerr << "Cannot map input file. Error: " << GetLastError() << endl;
        CloseHandle(hInputMapping);
        CloseHandle(hInputFile);
        cin.get(); cin.get();
        return -1;
    }

    // 4. Вычисляем размер выходного файла (удвоенный для UTF-16)
    DWORD outputSize = inputSize * 2;

    // 5. Создаём выходной файл
    HANDLE hOutputFile = CreateFileA(
        outputFileName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hOutputFile == INVALID_HANDLE_VALUE)
    {
        cerr << "Cannot create output file. Error: " << GetLastError() << endl;
        UnmapViewOfFile(pInputData);
        CloseHandle(hInputMapping);
        CloseHandle(hInputFile);
        cin.get(); cin.get();
        return -1;
    }

    // 6. Устанавливаем размер выходного файла
    SetFilePointer(hOutputFile, outputSize, NULL, FILE_BEGIN);
    SetEndOfFile(hOutputFile);
    SetFilePointer(hOutputFile, 0, NULL, FILE_BEGIN);

    // 7. Отображаем выходной файл в память
    HANDLE hOutputMapping = CreateFileMapping(
        hOutputFile,
        NULL,
        PAGE_READWRITE,
        0,
        outputSize,
        NULL
    );

    if (hOutputMapping == NULL)
    {
        cerr << "Cannot create file mapping for output. Error: " << GetLastError() << endl;
        CloseHandle(hOutputFile);
        UnmapViewOfFile(pInputData);
        CloseHandle(hInputMapping);
        CloseHandle(hInputFile);
        cin.get(); cin.get();
        return -1;
    }

    LPWSTR pOutputData = (LPWSTR)MapViewOfFile(
        hOutputMapping,
        FILE_MAP_WRITE,
        0,
        0,
        outputSize
    );

    if (pOutputData == NULL)
    {
        cerr << "Cannot map output file. Error: " << GetLastError() << endl;
        CloseHandle(hOutputMapping);
        CloseHandle(hOutputFile);
        UnmapViewOfFile(pInputData);
        CloseHandle(hInputMapping);
        CloseHandle(hInputFile);
        cin.get(); cin.get();
        return -1;
    }

    // 8. Преобразование ASCII -> UTF-16 (Unicode)
    // Используем CP_ACP (текущая кодовая страница ANSI) для ASCII
    int charsConverted = MultiByteToWideChar(
        CP_ACP,           // кодовая страница (ANSI/ASCII)
        0,                // флаги
        pInputData,       // входные данные (ASCII)
        inputSize,        // длина входных данных в байтах
        pOutputData,      // выходной буфер (UTF-16)
        outputSize / 2    // размер выходного буфера в WCHAR
    );

    if (charsConverted == 0)
    {
        cerr << "Conversion failed. Error: " << GetLastError() << endl;
        UnmapViewOfFile(pOutputData);
        CloseHandle(hOutputMapping);
        UnmapViewOfFile(pInputData);
        CloseHandle(hInputMapping);
        CloseHandle(hOutputFile);
        CloseHandle(hInputFile);
        cin.get(); cin.get();
        return -1;
    }

    // 9. Подрезаем выходной файл до реального размера
    DWORD realOutputSize = charsConverted * sizeof(WCHAR);
    SetFilePointer(hOutputFile, realOutputSize, NULL, FILE_BEGIN);
    SetEndOfFile(hOutputFile);

    // 10. Закрываем все дескрипторы
    UnmapViewOfFile(pOutputData);
    CloseHandle(hOutputMapping);
    UnmapViewOfFile(pInputData);
    CloseHandle(hInputMapping);
    CloseHandle(hOutputFile);
    CloseHandle(hInputFile);

    // 11. Результат
    cout << "\nConversion completed successfully!" << endl;
    cout << "Input file size: " << inputSize << " bytes" << endl;
    cout << "Output file size: " << realOutputSize << " bytes (UTF-16)" << endl;
    cout << "Characters converted: " << charsConverted << endl;

    cin.get(); cin.get();
    return 0;
}