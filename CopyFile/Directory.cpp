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