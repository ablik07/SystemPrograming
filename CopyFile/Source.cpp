#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

// Имена объектов (должны совпадать с серверными)
const char* MAPPING_NAME = "Global\\FileMapping";
const char* MUTEX_NAME = "Global\\Mutex";
const int BUFFER_SIZE = 4096;

int main()
{
    cout << "=== CLIENT ===" << endl;
    cout << "PID: " << GetCurrentProcessId() << endl;

    // 1. Открываем существующий мьютекс
    HANDLE hMutex = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, MUTEX_NAME);
    if (hMutex == NULL)
    {
        cerr << "OpenMutex failed. Make sure server is running." << endl;
        cerr << "Error: " << GetLastError() << endl;
        return -1;
    }

    // 2. Открываем существующее отображение файла
    HANDLE hMapping = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, MAPPING_NAME);
    if (hMapping == NULL)
    {
        cerr << "OpenFileMapping failed. Make sure server is running." << endl;
        cerr << "Error: " << GetLastError() << endl;
        CloseHandle(hMutex);
        return -1;
    }

    // 3. Отображаем в адресное пространство
    char* pBuffer = (char*)MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, BUFFER_SIZE);
    if (pBuffer == NULL)
    {
        cerr << "MapViewOfFile failed. Error: " << GetLastError() << endl;
        CloseHandle(hMapping);
        CloseHandle(hMutex);
        return -1;
    }

    cout << "Connected to server." << endl;
    cout << "Enter file names to count spaces, or 'exit' to quit." << endl;
    cout << "----------------------------------------" << endl;

    string command;
    bool running = true;

    while (running)
    {
        cout << "> ";
        getline(cin, command);

        if (command.empty()) continue;

        // Захватываем мьютекс (ждём, пока сервер освободит его)
        WaitForSingleObject(hMutex, INFINITE);

        // Записываем команду в отображение
        strcpy_s(pBuffer, BUFFER_SIZE, command.c_str());

        // Освобождаем мьютекс (сервер теперь может прочитать команду)
        ReleaseMutex(hMutex);

        if (command == "exit")
        {
            cout << "Exit command sent. Waiting for server..." << endl;
            // Даём серверу время на обработку
            Sleep(500);
            break;
        }

        // Ждём немного, пока сервер обработает команду
        Sleep(500);

        // Захватываем мьютекс, чтобы прочитать результат
        WaitForSingleObject(hMutex, INFINITE);

        // Читаем результат из отображения
        string result(pBuffer);
        cout << "Server response: " << result << endl;

        // Освобождаем мьютекс
        ReleaseMutex(hMutex);

        cout << "----------------------------------------" << endl;
    }

    // 4. Очистка
    UnmapViewOfFile(pBuffer);
    CloseHandle(hMapping);
    CloseHandle(hMutex);

    cout << "Client terminated." << endl;
    return 0;
}