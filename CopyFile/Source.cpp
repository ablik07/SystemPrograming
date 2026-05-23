#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

const char* EVENT_NAME = "Global\\PipeSyncEvent";

int main(int argc, char* argv[])
{
    cout << "=== CLIENT ===" << endl;
    cout << "PID: " << GetCurrentProcessId() << endl;

    // 1. Проверка аргументов командной строки
    if (argc != 3)
    {
        cerr << "Usage: client_pipe.exe <readHandle> <writeHandle>" << endl;
        cerr << "This program should be started by the server." << endl;
        return -1;
    }

    // 2. Преобразуем дескрипторы из командной строки
    HANDLE hReadPipe = (HANDLE)atoi(argv[1]);
    HANDLE hWritePipe = (HANDLE)atoi(argv[2]);

    cout << "Client: read handle = " << (int)hReadPipe << endl;
    cout << "Client: write handle = " << (int)hWritePipe << endl;

    // 3. Открываем существующее событие
    HANDLE hEvent = OpenEventA(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, EVENT_NAME);
    if (!hEvent)
    {
        cerr << "Client: OpenEvent failed. Error: " << GetLastError() << endl;
        return -1;
    }

    cout << "Client connected to server. Starting data exchange..." << endl;
    cout << "----------------------------------------" << endl;

    // 4. Двусторонний обмен данными
    int received;
    DWORD bytesRead;

    while (true)
    {
        // Ожидаем сигнала от сервера (готовность к чтению)
        WaitForSingleObject(hEvent, INFINITE);

        // Читаем число от сервера
        if (!ReadFile(hReadPipe, &received, sizeof(received), &bytesRead, NULL))
        {
            cerr << "Client: ReadFile failed. Error: " << GetLastError() << endl;
            break;
        }

        // Проверка на завершение
        if (received == -1)
        {
            cout << "Client received termination signal." << endl;
            break;
        }

        cout << "Client received: " << received << endl;

        // Обрабатываем: отправляем обратно число * 10
        int response = received * 10;

        // Сбрасываем событие — теперь сервер может читать
        ResetEvent(hEvent);

        // Отправляем ответ серверу
        DWORD bytesWritten;
        if (!WriteFile(hWritePipe, &response, sizeof(response), &bytesWritten, NULL))
        {
            cerr << "Client: WriteFile failed. Error: " << GetLastError() << endl;
            break;
        }
        cout << "Client sent: " << response << endl;

        // Сигнализируем серверу, что запись завершена
        SetEvent(hEvent);
    }

    cout << "----------------------------------------" << endl;
    cout << "Client finished." << endl;

    // 5. Закрываем дескрипторы
    CloseHandle(hReadPipe);
    CloseHandle(hWritePipe);
    CloseHandle(hEvent);

    return 0;
}