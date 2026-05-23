#include <windows.h>
#include <iostream>

using namespace std;

int main()
{
    // создаем каталог
    if (!CreateDirectory("C:\\Users\\karolina\\Downloads\\demo_dir", NULL))
    {
        cerr << "Create directory failed." << endl;
        << "The last error code: " << GetLastError() << endl;
        cout << "Press any key to finish.";
        cin.get();
        return 0;
    }

    cout << "The directory is created." << endl;
    return 0;
}

// Пример создания подкаталога

#include <windows.h>
#include <iostream>

using namespace std;

int main()
{
    // создаем подкаталог
    if (!CreateDirectoryEx("C:\\Users\\karolina\\Downloads\\demo", "C:\\Users\\karolina\\Downloads\\demo_1", NULL))
    {
        cerr << "Create directory failed." << endl
            << "The last error code: " << GetLastError() << endl;
        cout << "Press any key to finish.";
        cin.get();
        return 0;
    }

    cout << "The subdirectory is created." << endl;
    return 0;
}

// Пример поиска файлов в каталоге

#include <windows.h>
#include <iostream>

using namespace std;

int main()
{
    HANDLE hFindFile;
    WIN32_FIND_DATA fd;

    // находим первый файл
    hFindFile = FindFirstFile("C:\\Users\\karolina\\Downloads\\demo_\\*", &fd);
    if (hFindFile == INVALID_HANDLE_VALUE)
    {
        cerr << "Find first file failed." << endl
            << "The last error code: " << GetLastError() << endl;
        cout << "Press any key to finish.";
        cin.get();
        return 0;
    }

    // выводим на консоль имя первого файла
    cout << "The first file name: " << fd.cFileName << endl;

    // находим следующий файл и выводим на консоль его имя
    while (FindNextFile(hFindFile, &fd))
    {
        cout << "The next file name: " << fd.cFileName << endl;
        // закрываем дескриптор поиска
        FindClose(hFindFile);
        return 0;
    }
}