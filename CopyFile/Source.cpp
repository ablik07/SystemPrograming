#include <windows.h>
#include <iostream>

using namespace std;

// Функция потока (бесконечный цикл)
DWORD WINAPI ThreadFunc(LPVOID data)
{
    int counter = 0;
    while (true)
    {
        cout << "Thread working... counter = " << counter++ << endl;
        Sleep(1000);
    }
    return 0;
}

int main()
{
    HANDLE hThread;
    DWORD threadID;

    hThread = CreateThread(
        NULL,              // атрибуты безопасности
        0,                 // размер стека
        ThreadFunc,        // функция потока
        NULL,              // параметр
        0,                 // флаги
        &threadID          // ID потока
    );

    if (hThread == NULL)
    {
        cerr << "CreateThread failed. Error: " << GetLastError() << endl;
        cin.get();
        return -1;
    }

    cout << "Thread started. ID: " << threadID << endl;
    Sleep(3000);  // даём потоку поработать 3 секунды

    // Принудительное завершение потока
    cout << "Terminating thread..." << endl;
    if (!TerminateThread(hThread, 1))
    {
        cerr << "TerminateThread failed. Error: " << GetLastError() << endl;
    }
    else
    {
        cout << "Thread terminated." << endl;
    }

    CloseHandle(hThread);
    cin.get();
    return 0;
}