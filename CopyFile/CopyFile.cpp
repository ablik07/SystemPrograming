#include <windows.h>
#include <iostream>
#include <process.h>   // для _beginthreadex, _endthreadex

using namespace std;

// Функция потока
unsigned int __stdcall ThreadFunc(void* data)
{
    int num = *(int*)data;
    for (int i = 1; i <= 5; i++)
    {
        cout << "Thread " << num << ": iteration " << i << endl;
        Sleep(500);
    }
    cout << "Thread " << num << " finished." << endl;
    return 0;
}

int main()
{
    HANDLE hThread;
    unsigned int threadID;
    int param = 1;

    // Создание потока через _beginthreadex
    hThread = (HANDLE)_beginthrexe(
        &ThreadFunc,       // адрес функции потока
        0,                 // размер стека (0 = по умолчанию)
        &param,            // параметр для потока
        0,                 // флаги создания (0 = сразу запуск)
        &threadID          // идентификатор потока
    );

    if (hThread == NULL)
    {
        cerr << "_beginthreadex failed. Error: " << GetLastError() << endl;
        cin.get();
        return -1;
    }

    cout << "Thread created. ID: " << threadID << endl;

    // Ожидание завершения потока
    WaitForSingleObject(hThread, INFINITE);

    CloseHandle(hThread);
    cout << "Main thread finished." << endl;
    cin.get();
    return 0;
}