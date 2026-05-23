#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Имена объектов
const char* MAPPING_NAME = "Global\\FileMapping";
const char* EVENT_COMMAND = "Global\\CommandReadyEvent";
const char* EVENT_RESULT = "Global\\ResultReadyEvent";
const char* EVENT_EXIT = "Global\\ExitEvent";

const int BUFFER_SIZE = 4096;

int main()
{
    cout << "=== SERVER ===" << endl;
    cout << "PID: " << GetCurrentProcessId() << endl;

    // 1. Создаём отображение файла в памяти
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
        return -1;
    }

    // 2. Отображаем в адресное пространство
    char* pBuffer = (char*)MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, BUFFER_SIZE);
    if (pBuffer == NULL)
    {
        cerr << "MapViewOfFile failed. Error: " << GetLastError() << endl;
        CloseHandle(hMapping);
        return -1;
    }

    // 3. Создаём события
    HANDLE hCommandEvent = CreateEventA(NULL, FALSE, FALSE, EVENT_COMMAND);
    HANDLE hResultEvent = CreateEventA(NULL, FALSE, FALSE, EVENT_RESULT);
    HANDLE hExitEvent = CreateEventA(NULL, FALSE, FALSE, EVENT_EXIT);

    if (!hCommandEvent || !hResultEvent || !hExitEvent)
    {
        cerr << "CreateEvent failed. Error: " << GetLastError() << endl;
        UnmapViewOfFile(pBuffer);
        CloseHandle(hMapping);
        return -1;
    }

    cout << "Server ready. Waiting for commands..." << endl;
    cout << "----------------------------------------" << endl;

    // 4. Основной цикл
    HANDLE hEvents[2] = { hCommandEvent, hExitEvent };
    bool running = true;

    while (running)
    {
        // Ожидаем команду или сигнал завершения
        DWORD waitResult = WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);

        if (waitResult == WAIT_OBJECT_0 + 1) // ExitEvent
        {
            cout << "Exit signal received. Shutting down..." << endl;
            break;
        }
        else if (waitResult == WAIT_OBJECT_0) // CommandEvent
        {
            // Читаем команду из отображения
            string command(pBuffer);
            cout << "Received command: " << command << endl;

            if (command == "exit")
            {
                // Клиент запросил завершение
                SetEvent(hExitEvent); // сигнал для клиента (опционально)
                break;
            }

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
            strncpy_s(pBuffer, BUFFER_SIZE, result.c_str(), _TRUNCATE);

            // Сигнализируем клиенту, что результат готов
            SetEvent(hResultEvent);

            cout << "Result: " << result << endl;
            cout << "----------------------------------------" << endl;
        }
    }

    // 5. Очистка
    UnmapViewOfFile(pBuffer);
    CloseHandle(hMapping);
    CloseHandle(hCommandEvent);
    CloseHandle(hResultEvent);
    CloseHandle(hExitEvent);

    cout << "Server terminated." << endl;
    return 0;
}