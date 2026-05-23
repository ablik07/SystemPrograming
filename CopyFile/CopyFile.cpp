#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
    // 1. Проверка количества аргументов командной строки
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <input_file> <max_replacements>" << endl;
        cerr << "Example: " << argv[0] << " text.txt 10" << endl;
        return -1;
    }

    const char* inputFileName = argv[1];
    int maxReplacements = atoi(argv[2]);

    if (maxReplacements <= 0)
    {
        cerr << "Error: number of replacements must be positive." << endl;
        return -1;
    }

    // 2. Открытие входного файла
    ifstream inFile(inputFileName);
    if (!inFile.is_open())
    {
        cerr << "Error: cannot open input file '" << inputFileName << "'" << endl;
        return -1;
    }

    // 3. Формирование имени выходного файла (такое же, как входной)
    string outputFileName = inputFileName;

    // 4. Открытие выходного файла (перезапись)
    ofstream outFile(outputFileName);
    if (!outFile.is_open())
    {
        cerr << "Error: cannot create output file '" << outputFileName << "'" << endl;
        inFile.close();
        return -1;
    }

    // 5. Обработка файла построчно
    string line;
    int totalReplacements = 0;

    while (getline(inFile, line))
    {
        string processedLine = line;

        // Проход по строке и поиск пар одинаковых символов
        for (size_t i = 0; i < processedLine.length() - 1; i++)
        {
            if (totalReplacements >= maxReplacements)
                break;

            if (processedLine[i] == processedLine[i + 1])
            {
                processedLine[i + 1] = ' ';
                totalReplacements++;
                i++; // пропускаем следующий символ (чтобы не считать одну и ту же пару повторно)
            }
        }

        outFile << processedLine << endl;

        if (totalReplacements >= maxReplacements)
            break;
    }

    // 6. Закрытие файлов
    inFile.close();
    outFile.close();

    // 7. Вывод результата
    cout << "Processing completed." << endl;
    cout << "Replacements performed: " << totalReplacements << endl;

    return totalReplacements;
}