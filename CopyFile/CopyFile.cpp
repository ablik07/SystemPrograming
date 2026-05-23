#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Имена объектов
const char* MAPPING_NAME = "Global\\FileMapping";
const char* MUTEX_NAME = "Global\\Mutex";
const int BUFFER_SIZE = 4096;

int main()
{
    cout << "=== SERVER ===" << endl;
    cout << "PID: " << GetCurrentProcessId() << endl;

    // 1. Создаём мьютекс
    HANDLE hMutex = CreateMutexA(NULL, FALSE, MUTEX_NAME);
    if (hMutex == NULL)
    {
        cerr << "CreateMutex failed. Error: " << GetLastError() << endl;
        return -1;
    }

    // 2. Создаём отображение файла в памяти
    HANDLE hMapping = CreateFileMappingA(
        INVALID_HANDLE_VALUE,   // не связан с файлом на диске
        NULL,
        PAGE_READWRITE,
        0,
        BUFFER_SIZE,
        MAPPING_NAME
    );

    if (hMapping == NULL)
    {
        cerr << "CreateFileMapping failed. Error: " << GetLastError() << endl;
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

    cout << "Server ready. Waiting for commands..." << endl;
    cout << "----------------------------------------" << endl;

    bool running = true;
    string lastCommand = "";

    while (running)
    {
        // Захватываем мьютекс (ждём, пока клиент не освободит его)
        WaitForSingleObject(hMutex, INFINITE);

        // Читаем команду из отображения
        string command(pBuffer);

        // Если команда новая и не пустая
        if (!command.empty() && command != lastCommand)
        {
            lastCommand = command;
            cout << "Received command: " << command << endl;

            if (command == "exit")
            {
                // Команда завершения
                strcpy_s(pBuffer, BUFFER_SIZE, "Server shutting down...");
                running = false;
            }
            else
            {
                // Обработка команды: подсчёт пробелов в файле
                string result;
                int spaceCount = -1;

                ifstream file(command);
                if (file.is_open())
                {
                    spaceCount = 0;
                    char ch;
                    while (file.get(ch))
                    {
                        if (ch == ' ') spaceCount++;
                    }
                    file.close();

                    char buffer[256];
                    sprintf_s(buffer, sizeof(buffer), "File '%s' spaces: %d", command.c_str(), spaceCount);
                    result = buffer;
                }
                else
                {
                    result = "ERROR: Cannot open file '" + command + "'";
                }

                // Записываем результат в отображение
                strcpy_s(pBuffer, BUFFER_SIZE, result.c_str());

                cout << "Result: " << result << endl;
                cout << "--------------------------------------" << endl;
            }
        }

        // Освобождаем мьютекс (сигнализируем клиенту, что результат готов)
        ReleaseMutex(hMutex);

        // Небольшая задержка, чтобы не нагружать процессор
        Sleep(100);
    }

    // 4. Очистка
    UnmapViewOfFile(pBuffer);
    CloseHandle(hMapping);
    CloseHandle(hMutex);

    cout << "Server terminated." << endl;
    return 0;
}