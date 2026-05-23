#include <windows.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

int main()
{
    string fileName;
    cout << "Enter file name: ";
    cin >> fileName;

    // Чтение файла
    HANDLE hFile = CreateFileA(fileName.c_str(), GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        cout << "Error!" << endl;
        return -1;
    }

    // Если пустой - пишем пример
    if (GetFileSize(hFile, NULL) == 0)
    {
        const char* test = "5 2 8 1 9 3 7 4 6";
        DWORD written;
        WriteFile(hFile, test, (DWORD)strlen(test), &written, NULL);
        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    }

    // Читаем содержимое
    DWORD size = GetFileSize(hFile, NULL);
    char* buffer = new char[size + 1];
    DWORD read;
    ReadFile(hFile, buffer, size, &read, NULL);
    buffer[size] = 0;

    // Парсим числа
    vector<int> nums;
    stringstream ss(string(buffer));
    int x;
    while (ss >> x) nums.push_back(x);

    // Сортируем по убыванию
    sort(nums.begin(), nums.end(), greater<int>());

    // Формируем результат
    stringstream out;
    for (int n : nums) out << n << " ";

    // Записываем обратно
    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    SetEndOfFile(hFile);
    string result = out.str();
    WriteFile(hFile, result.c_str(), (DWORD)result.size(), &size, NULL);

    cout << "Result: " << result << endl;

    delete[] buffer;
    CloseHandle(hFile);

    return 0;
}