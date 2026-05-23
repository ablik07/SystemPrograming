
#include <windows.h>
#include <iostream>
#include <stdio.h>
using namespace std;

int main()
{
    HANDLE hFile; // дескриптор файла
    long n; // для номера записи
    long p; // для указателя позиции
    DWORD dwBytesRead; // количество прочитанных байт
    int m; // прочитанное число

    hFile = CreateFile(TEXT("test.txt"), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        cerr << "Create file failed." << endl << "The last error code: " << GetLastError() << endl;
        cout << "Press any key to finish.";
        cin.get();
        return 0;
    }

    // вводим номер нужной записи
    cout << "Input a number from 0 to 9: ";
    cin >> n;

    // сдвигаем указатель позиции файла
    p = SetFilePointer(hFile, n, NULL, FILE_BEGIN);
    if (p == -1)
    {
        cerr << "Set file pointer failed." << endl << "The last error code: " << GetLastError() << endl;
        CloseHandle(hFile);
        cout << "Press any key to finish.";
        cin.get();
        return 0;
    }

    cout << "File pointer: " << p << endl;

    // читаем данные из файла
    if (!ReadFile(hFile, &m, sizeof(m), &dwBytesRead, NULL))
    {
        cerr << "Read file failed." << endl
            << "The last error code: " << GetLastError() << endl;
        CloseHandle(hFile);
        cout << "Press any key to finish.";
        cin.get();
        return 0;
    }

    printf("The read number: %c ", m);
    CloseHandle(hFile);
    return 0;
}