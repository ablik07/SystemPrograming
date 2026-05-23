#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: parent.exe <file1> [file2] [file3] ..." << endl;
        cerr << "Each file will be processed by a child process (variant 11)." << endl;
        return -1;
    }

    int numFiles = argc - 1;
    HANDLE* hProcesses = new HANDLE[numFiles];
    DWORD* processIds = new DWORD[numFiles];

    cout << "Parent process started. PID: " << GetCurrentProcessId() << endl;
    cout << "Creating " << numFiles << " child processes...\n" << endl;

    // Создание дочерних процессов
    for (int i = 0; i < numFiles; i++)
    {
        // Формирование командной строки: child.exe <filename> <max_replacements>
        char cmdLine[512];
        sprintf_s(cmdLine, sizeof(cmdLine), "child.exe %s 100", argv[i + 1]);

        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        if (!CreateProcess(
            NULL,           // имя программы (из переменной окружения)
            cmdLine,        // командная строка
            NULL,           // атрибуты безопасности процесса
            NULL,           // атрибуты безопасности потока
            FALSE,          // наследование дескрипторов
            0,              // флаги создания
            NULL,           // переменные окружения
            NULL,           // текущий каталог
            &si,            // STARTUPINFO
            &pi))           // PROCESS_INFORMATION
        {
            cerr << "CreateProcess failed for file '" << argv[i + 1]
                << "'. Error: " << GetLastError() << endl;
            hProcesses[i] = NULL;
            processIds[i] = 0;
        }
        else
        {
            cout << "Child process created. PID: " << pi.dwProcessId
                << " for file: " << argv[i + 1] << endl;
            hProcesses[i] = pi.hProcess;
            processIds[i] = pi.dwProcessId;
            CloseHandle(pi.hThread); // дескриптор потока не нужен
        }
    }

    cout << "\nWaiting for all child processes to finish...\n" << endl;

    // Ожидание завершения всех дочерних процессов
    DWORD waitResult = WaitForMultipleObjects(numFiles, hProcesses, TRUE, INFINITE);

    if (waitResult == WAIT_FAILED)
    {
        cerr << "WaitForMultipleObjects failed. Error: " << GetLastError() << endl;
        return -1;
    }

    // Сбор результатов
    int totalReplacements = 0;
    cout << "\n--- Results ---" << endl;

    for (int i = 0; i < numFiles; i++)
    {
        if (hProcesses[i] != NULL)
        {
            DWORD exitCode;
            if (GetExitCodeProcess(hProcesses[i], &exitCode))
            {
                cout << "Process " << processIds[i] << " (file: " << argv[i + 1]
                    << ") returned: " << (int)exitCode << " replacements" << endl;
                totalReplacements += (int)exitCode;
            }
            else
            {
                cerr << "Failed to get exit code for process " << processIds[i] << endl;
            }
            CloseHandle(hProcesses[i]);
        }
    }

    cout << "\n======================================" << endl;
    cout << "All files processed. Total replacements: " << totalReplacements << endl;

    delete[] hProcesses;
    delete[] processIds;

    return 0;
}