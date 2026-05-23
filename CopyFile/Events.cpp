#include <windows.h>
#include <iostream>

using namespace std;

int main()
{
    if (!DeleteFile("C:\\Users\\karolina\\Downloads\\demo\\test.txt"))
    {
        cerr << "Delete file failed." << endl
            << "The last error code: " << GetLastError() << endl;
        cout << "Press any key to finish.";
        cin.get();
        return 0;
    }

    cout << "File deleted successfully." << endl;
    return 0;
}