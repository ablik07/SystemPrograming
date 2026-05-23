#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

const char* MAILSLOT_NAME = "\\\\.\\mailslot\\MyMailslot";

int main()
{
    string filename;
    int replacements;

    cout << "Enter filename: ";
    cin >> filename;
    cout << "Enter max replacements: ";
    cin >> replacements;

    HANDLE hMailslot = CreateFileA(MAILSLOT_NAME, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hMailslot == INVALID_HANDLE_VALUE) return -1;

    string request = filename + " " + to_string(replacements);
    DWORD bytesWritten;
    WriteFile(hMailslot, request.c_str(), (DWORD)request.size() + 1, &bytesWritten, NULL);
    CloseHandle(hMailslot);

    HANDLE hReadSlot = CreateMailslotA(MAILSLOT_NAME, 0, MAILSLOT_WAIT_FOREVER, NULL);
    if (hReadSlot == INVALID_HANDLE_VALUE) return -1;

    char response[512];
    DWORD bytesRead;
    ReadFile(hReadSlot, response, sizeof(response), &bytesRead, NULL);
    cout << "Server response: " << response << endl;

    CloseHandle(hReadSlot);
    return 0;
}