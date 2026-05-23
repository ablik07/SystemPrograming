//#include <windows.h>
//#include <iostream>
//
//using namespace std;
//
//int main()
//{
//    if (!DeleteFile("C:\\Users\\karolina\\Downloads\\demo\\test.txt"))
//    {
//        cerr << "Delete file failed." << endl
//            << "The last error code: " << GetLastError() << endl;
//        cout << "Press any key to finish.";
//        cin.get();
//        return 0;
//    }
//
//    cout << "File deleted successfully." << endl;
//    return 0;
//}
//

//#include <windows.h>
//#include <iostream>
//
//using namespace std;
//
//int main()
//{
//    // Переименование (перемещение) файла
//    if (!MoveFile("C:\\Users\\karolina\\Downloads\\demo\\old.txt",
//        "C:\\Users\\karolina\\Downloads\\demo\\new.txt"))
//    {
//        cerr << "Move file failed." << endl
//            << "The last error code: " << GetLastError() << endl;
//        cout << "Press any key to finish.";
//        cin.get();
//        return 0;
//    }
//
//    cout << "File moved/renamed successfully." << endl;
//    return 0;
//}

#include <windows.h>
#include <iostream>

using namespace std;

int main()
{
    // Создание каталога
    if (!CreateDirectory("C:\\Users\\karolina\\Downloads\\my_new_dir", NULL))
    {
        cerr << "Create directory failed." << endl
            << "The last error code: " << GetLastError() << endl;
        cout << "Press any key to finish.";
        cin.get();
        return 0;
    }

    cout << "Directory created successfully." << endl;

    // Удаление каталога (только пустой!)
    if (!RemoveDirectory("C:\\Users\\karolina\\Downloads\\my_new_dir"))
    {
        cerr << "Remove directory failed." << endl
            << "The last error code: " << GetLastError() << endl;
        cout << "Press any key to finish.";
        cin.get();
        return 0;
    }

    cout << "Directory removed successfully." << endl;
    return 0;
}
