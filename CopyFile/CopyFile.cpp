#include <windows.h>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

// Имя события для синхронизации
const char* EVENT_NAME = "Global\\PipeSyncEvent";

int main()
{
    cout << "=== SERVER ===" << endl;
    cout << "PID: " << GetCurrentProcessId() << endl;

    // 1. Создаём анонимный канал
    HANDLE hReadPipe, hWritePipe;
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;  // дескрипторы должны наследоваться

    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0))
    {
        cerr << "CreatePipe failed. Error: " << GetLastError() << endl;
        return -1;
    }

    // 2. Создаём событие для синхронизации
    HANDLE hEvent = CreateEventA(NULL, FALSE, TRUE, EVENT_NAME);
    if (!hEvent)
    {
        cerr << "CreateEvent failed. Error: " << GetLastError() << endl;
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        return -1;
    }

    // 3. Подготавливаем командную строку для клиента
    // Передаём дескрипторы как числа
    char cmdLine[512];
    sprintf_s(cmdLine, sizeof(cmdLine),
        "client_pipe.exe %d %d",
        (int)hReadPipe, (int)hWritePipe);

    // 4. Настраиваем STARTUPINFO для дочернего процесса
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // 5. Создаём дочерний процесс (клиента)
    cout << "Starting client process..." << endl;

    if (!CreateProcess(
        NULL,               // имя программы
        cmdLine,            // командная строка с дескрипторами
        NULL,               // атрибуты процесса
        NULL,               // атрибуты потока
        TRUE,               // наследовать дескрипторы!
        CREATE_NEW_CONSOLE, // новая консоль для клиента
        NULL,               // окружение
        NULL,               // текущий каталог
        &si,
        &pi))
    {
        cerr << "CreateProcess failed. Error: " << GetLastError() << endl;
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        CloseHandle(hEvent);
        return -1;
    }

    // Закрываем дескрипторы процесса и потока клиента (они не нужны серверу)
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    cout << "Client started. Beginning data exchange..." << endl;
    cout << "----------------------------------------" << endl;

    // 6. Двусторонний обмен данными
    // Сервер отправляет числа 0..4, затем читает ответы

    for (int i = 0; i < 5; i++)
    {
        // Ожидаем, пока клиент не прочитает предыдущие данные
        WaitForSingleObject(hEvent, INFINITE);

        // Отправляем число клиенту
        DWORD bytesWritten;
        if (!WriteFile(hWritePipe, &i, sizeof(i), &bytesWritten, NULL))
        {
            cerr << "Server: WriteFile failed. Error: " << GetLastError() << endl;
            break;
        }
        cout << "Server sent: " << i << endl;

        // Сбрасываем событие — теперь клиент может читать
        ResetEvent(hEvent);

        // Читаем ответ от клиента
        int response;
        DWORD bytesRead;
        if (!ReadFile(hReadPipe, &response, sizeof(response), &bytesRead, NULL))
        {
            cerr << "Server: ReadFile failed. Error: " << GetLastError() << endl;
            break;
        }
        cout << "Server received: " << response << endl;

        // Сигнализируем клиенту, что чтение завершено
        SetEvent(hEvent);

        cout << "---" << endl;
    }

    // 7. Отправляем сигнал завершения (-1)
    int endSignal = -1;
    DWORD bytesWritten;
    WaitForSingleObject(hEvent, INFINITE);
    WriteFile(hWritePipe, &endSignal, sizeof(endSignal), &bytesWritten, NULL);
    ResetEvent(hEvent);
    SetEvent(hEvent);  // чтобы клиент тоже завершился

    cout << "---------------------------------------" << endl;
    cout << "Server: Exchange completed." << endl;

    // 8. Закрываем дескрипторы
    CloseHandle(hReadPipe);
    CloseHandle(hWritePipe);
    CloseHandle(hEvent);

    return 0;
}