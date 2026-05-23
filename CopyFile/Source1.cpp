#include <windows.h>
#include <iostream>

using namespace std;

DWORD WINAPI ThreadFunc(LPVOID data)
{
    int counter = 0;
    while (true)
    {
        cout << "Thread is running... counter = " << counter++ << endl;
        Sleep(1000);
    }
    return 0;
}

int main()
{
    HANDLE hThread;
    DWORD threadID;

    hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &threadID);

    if (hThread == NULL)
    {
        cerr << "CreateThread failed. Error: " << GetLastError() << endl;
        cin.get();
        return -1;
    }

    cout << "Thread started. ID: " << threadID << endl;
    Sleep(2000);

    // Приостановка потока
    cout << "\nSuspending thread..." << endl;
    if (SuspendThread(hThread) == (DWORD)-1)
    {
        cerr << "SuspendThread failed. Error: " << GetLastError() << endl;
    }
    else
    {
        cout << "Thread suspended." << endl;
    }

    Sleep(3000);  // поток не работает в это время

    // Возобновление потока
    cout << "\nResuming thread..." << endl;
    if (ResumeThread(hThread) == (DWORD)-1)
    {
        cerr << "ResumeThread failed. Error: " << GetLastError() << endl;
    }
    else
    {
        cout << "Thread resumed." << endl;
    }

    Sleep(3000);

    // Завершаем поток принудительно для выхода из программы
    TerminateThread(hThread, 0);
    CloseHandle(hThread);

    cin.get();
    return 0;
}