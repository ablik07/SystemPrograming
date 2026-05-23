#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

const char* PIPE_NAME = "\\\\.\\pipe\\MyNamedPipe";

int main()
{
    HANDLE hPipe = CreateFileA(PIPE_NAME, GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, 0, NULL);

    if (hPipe == INVALID_HANDLE_VALUE) return -1;

    string filename;
    int replacements;

    cout << "Enter filename: ";
    cin >> filename;
    cout << "Enter max replacements: ";
    cin >> replacements;

    string request = filename + " " + to_string(replacements);
    DWORD bytesWritten;
    WriteFile(hPipe, request.c_str(), (DWORD)request.size() + 1, &bytesWritten, NULL);

    char response[512];
    DWORD bytesRead;
    ReadFile(hPipe, response, sizeof(response), &bytesRead, NULL);

    cout << "Server response: " << response << endl;

    CloseHandle(hPipe);
    return 0;
}