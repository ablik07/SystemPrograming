#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
    // Проверка аргументов
    if (argc != 3)
    {
        cerr << "Usage: child.exe <input_file> <max_replacements>" << endl;
        return -1;
    }

    const char* inputFileName = argv[1];
    int maxReplacements = atoi(argv[2]);

    if (maxReplacements <= 0)
    {
        cerr << "Error: number of replacements must be positive." << endl;
        return -1;
    }

    // Открытие входного файла
    ifstream inFile(inputFileName);
    if (!inFile.is_open())
    {
        cerr << "Error: cannot open input file '" << inputFileName << "'" << endl;
        return -1;
    }

    // Выходной файл с тем же именем
    string outputFileName = inputFileName;
    ofstream outFile(outputFileName);
    if (!outFile.is_open())
    {
        cerr << "Error: cannot create output file '" << outputFileName << "'" << endl;
        inFile.close();
        return -1;
    }

    // Обработка файла
    string line;
    int totalReplacements = 0;

    while (getline(inFile, line) && totalReplacements < maxReplacements)
    {
        string processedLine = line;

        for (size_t i = 0; i < processedLine.length() - 1; i++)
        {
            if (totalReplacements >= maxReplacements)
                break;

            if (processedLine[i] == processedLine[i + 1])
            {
                processedLine[i + 1] = ' ';
                totalReplacements++;
                i++;
            }
        }

        outFile << processedLine << endl;
    }

    inFile.close();
    outFile.close();

    // Вывод результата (родительский процесс прочитает через GetExitCodeProcess)
    cout << "Process " << GetCurrentProcessId()
        << ": File '" << inputFileName
        << "' processed. Replacements: " << totalReplacements << endl;

    return totalReplacements;
}