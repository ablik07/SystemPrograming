#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const char* PIPE_NAME = "\\\\.\\pipe\\MyNamedPipe";

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
    HANDLE hPipe = CreateNamedPipeA(PIPE_NAME, PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        1, 512, 512, 0, NULL);

    if (hPipe == INVALID_HANDLE_VALUE) return -1;

    cout << "Server started. PID: " << GetCurrentProcessId() << endl;

    char buffer[512];
    DWORD bytesRead;

    while (true)
    {
        ConnectNamedPipe(hPipe, NULL);

        if (ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL))
        {
            buffer[bytesRead] = '\0';
            string filename(buffer);

            if (filename == "exit") break;

            int replacements = atoi(strrchr(filename, ' ') + 1);
            filename = filename.substr(0, filename.find(' '));

            int result = ProcessFile(filename.c_str(), replacements);

            char response[512];
            if (result == -1)
                sprintf_s(response, "ERROR: Cannot open file '%s'", filename.c_str());
            else
                sprintf_s(response, "OK: %d replacements", result);

            WriteFile(hPipe, response, (DWORD)strlen(response) + 1, &bytesRead, NULL);
        }

        DisconnectNamedPipe(hPipe);
    }

    CloseHandle(hPipe);
    return 0;
}