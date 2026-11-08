#include "Program_Loader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
using namespace std;

bool ProgramLoader::loadProgram(string fileName, int startAddress, Memory& memory)
{
    ifstream file(fileName);

    if (!file.is_open())
    {
        cout << "Error: Could not open program file!" << endl;
        return false;
    }

    string line;
    int instructionCount = 0;

    while (getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }

        if (line[0] == ';')
        {
            continue;
        }


        if (line.length() < 4)
        {
            cout << "Error: Invalid instruction!" << endl;
            return false;
        }

        for (int i = 0; i < 4; i++)
        {
            if (!((line[i] >= '0' && line[i] <= '9') ||
                (line[i] >= 'A' && line[i] <= 'F') ||
                (line[i] >= 'a' && line[i] <= 'f')))
            {
                cout << "Error: Invalid hexadecimal instruction!" << endl;
                return false;
            }
        }

        char opcode = toupper(line[0]);
        switch (opcode) {
        case '1': case '2' : case '3': case '5': case '6': case '7': case '8': case '9': case 'B':
        break;
        case '4':
        if (line[1] != '0') {
            cout <<"invalid format"<< endl;
            return false;
        }
        break;
        case 'A':
        if (line[2] != '0') {
            cout <<"invalid format"<< endl; 
            return false;
        }
        break;
        case 'C':
        if (line[1] != '0' || line[2] != '0' || line[3] != '0') {
            cout <<"invalid format"<< endl;
            return false;
        }
        break;
        default:
        return false;
    }

        instructionCount++;
    }

    if (instructionCount == 0)
    {
        cout << "Error: Program file is empty!" << endl;
        return false;
    }

    if (startAddress < 0 || startAddress >= 255 || startAddress % 2 != 0)
    {
        cout << "Error: Invalid starting address!" << endl;
        return false;
    }

    if (startAddress + (instructionCount * 2) > 256)
    {
        cout << "Error: Program exceeds memory capacity!" << endl;
        return false;
    }

    file.clear();
    file.seekg(0);

    int currentAddress = startAddress;

    while (getline(file, line))
    {
        if (line.empty() || line[0] == ';')
        {
            continue;
        }

        int firstByte = stoi(line.substr(0, 2), nullptr, 16);
        int secondByte = stoi(line.substr(2, 2), nullptr, 16);

        memory.write(currentAddress, firstByte);
        memory.write(currentAddress + 1, secondByte);

        currentAddress += 2;
    }

    return true;
}