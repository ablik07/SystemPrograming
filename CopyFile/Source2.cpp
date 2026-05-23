#include <windows.h>
#include <iostream>

using namespace std;

DWORD WINAPI ThreadFunc(LPVOID data)
{
    HANDLE hCurrentThread = GetCurrentThread();  // псевдодескриптор
    DWORD realID = GetThreadId(hCurrentThread);  // получение реального ID

    cout << "   Child thread: pseudo-handle = " << hCurrentThread
        << ", real thread ID = " << realID << endl;

    // Преобразование псевдодескриптора в реальный (дублирование)
    HANDLE hRealHandle;
    DuplicateHandle(GetCurrentProcess(),
        hCurrentThread,
        GetCurrentProcess(),
        &hRealHandle,
        0,
        FALSE,
        DUPLICATE_SAME_ACCESS);

    cout << "   Child thread: duplicated real handle = " << hRealHandle << endl;

    CloseHandle(hRealHandle);
    return 0;
}

int main()
{
    // Псевдодескриптор главного потока
    HANDLE hMainPseudo = GetCurrentThread();
    DWORD mainRealID = GetThreadId(hMainPseudo);

    cout << "Main thread: pseudo-handle = " << hMainPseudo
        << ", real thread ID = " << mainRealID << endl;

    // Создаём дочерний поток
    HANDLE hThread;
    DWORD threadID;
    hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &threadID);

    if (hThread == NULL)
    {
        cerr << "CreateThread failed. Error: " << GetLastError() << endl;
        cin.get();
        return -1;
    }

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    cin.get();
    return 0;
}