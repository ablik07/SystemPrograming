#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

// Имена объектов (должны совпадать с серверными)
const char* MAPPING_NAME = "Global\\FileMapping";
const char* EVENT_COMMAND = "Global\\CommandReadyEvent";
const char* EVENT_RESULT = "Global\\ResultReadyEvent";
const char* EVENT_EXIT = "Global\\ExitEvent";

const int BUFFER_SIZE = 4096;

int main()
{
    cout << "=== CLIENT ===" << endl;
    cout << "PID: " << GetCurrentProcessId() << endl;

    // 1. Открываем существующее отображение файла
    HANDLE hMapping = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, MAPPING_NAME);
    if (hMapping == NULL)
    {
        cerr << "OpenFileMapping failed. Make sure server is running." << endl;
        cerr << "Error: " << GetLastError() << endl;
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

    // 3. Открываем события
    HANDLE hCommandEvent = OpenEventA(EVENT_MODIFY_STATE, FALSE, EVENT_COMMAND);
    HANDLE hResultEvent = OpenEventA(SYNCHRONIZE, FALSE, EVENT_RESULT);
    HANDLE hExitEvent = OpenEventA(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, EVENT_EXIT);

    if (!hCommandEvent || !hResultEvent || !hExitEvent)
    {
        cerr << "OpenEvent failed. Make sure server is running." << endl;
        cerr << "Error: " << GetLastError() << endl;
        UnmapViewOfFile(pBuffer);
        CloseHandle(hMapping);
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

        // Записываем команду в отображение
        strncpy_s(pBuffer, BUFFER_SIZE, command.c_str(), _TRUNCATE);

        // Сигнализируем серверу о новой команде
        SetEvent(hCommandEvent);

        if (command == "exit")
        {
            // Ждём сигнала завершения от сервера
            WaitForSingleObject(hExitEvent, INFINITE);
            cout << "Server confirmed exit. Closing..." << endl;
            break;
        }

        // Ожидаем результат от сервера
        DWORD waitResult = WaitForSingleObject(hResultEvent, 5000);

        if (waitResult == WAIT_OBJECT_0)
        {
            // Читаем результат из отображения
            string result(pBuffer);
            cout << "Server response: " << result << endl;
        }
        else if (waitResult == WAIT_TIMEOUT)
        {
            cout << "Timeout waiting for server response." << endl;
        }
        else
        {
            cerr << "Wait failed. Error: " << GetLastError() << endl;
            break;
        }

        cout << "---------------------------------------" << endl;
    }

    // Очистка
    UnmapViewOfFile(pBuffer);
    CloseHandle(hMapping);
    CloseHandle(hCommandEvent);
    CloseHandle(hResultEvent);
    CloseHandle(hExitEvent);

    return 0;
}