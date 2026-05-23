#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const char* MAILSLOT_NAME = "\\\\.\\mailslot\\MyMailslot";

int ProcessFile(const char* filename, int maxReplacements)
{
    ifstream inFile(filename);
    if (!inFile.is_open()) return -1;

    string outFilename = string(filename) + ".out";
    ofstream outFile(outFilename);
    if (!outFile.is_open()) return -1;

    string line;
    int total = 0;

    while (getline(inFile, line) && total < maxReplacements)
    {
        for (size_t i = 0; i < line.length() - 1 && total < maxReplacements; i++)
        {
            if (line[i] == line[i + 1])
            {
                line[i + 1] = ' ';
                total++;
                i++;
            }
        }
        outFile << line << endl;
    }
    return total;
}

int main()
{
    HANDLE hMailslot = CreateMailslotA(MAILSLOT_NAME, 0, MAILSLOT_WAIT_FOREVER, NULL);
    if (hMailslot == INVALID_HANDLE_VALUE) return -1;

    cout << "Server started. PID: " << GetCurrentProcessId() << endl;

    char buffer[512];
    DWORD bytesRead;

    while (true)
    {
        if (!ReadFile(hMailslot, buffer, sizeof(buffer), &bytesRead, NULL)) break;
        buffer[bytesRead] = '\0';

        string data(buffer);
        if (data == "exit") break;

        string filename = data.substr(0, data.find(' '));
        int replacements = stoi(data.substr(data.find(' ') + 1));

        int result = ProcessFile(filename.c_str(), replacements);

        HANDLE hClient = CreateFileA(MAILSLOT_NAME, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hClient != INVALID_HANDLE_VALUE)
        {
            char response[256];
            if (result == -1)
                sprintf_s(response, "ERROR: Cannot open file '%s'", filename.c_str());
            else
                sprintf_s(response, "OK: %d replacements", result);

            DWORD bytesWritten;
            WriteFile(hClient, response, (DWORD)strlen(response) + 1, &bytesWritten, NULL);
            CloseHandle(hClient);
        }
    }

    CloseHandle(hMailslot);
    return 0;

}