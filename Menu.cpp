#include "Menu.h"
#include <iostream>
#include <limits>
#include <string>
#include <cstdint>

using namespace std;

void Menu::Run() {
    int choice = 0;

    while (true) {
        cout << "\n=========================================\n";
        cout << "       VOLE MACHINE SIMULATOR MENU       \n";
        cout << "=========================================\n";
        cout << "1. Load Program from File\n";
        cout << "2. Set Register Value Manually\n";
        cout << "3. Set Memory Cell Value Manually\n";
        cout << "4. Run Step-by-Step\n";
        cout << "5. Run All (Batch Execution)\n";
        cout << "6. Display Machine State\n";
        cout << "7. Exit\n";
        cout << "=========================================\n";
        cout << "Enter your choice (1-7): ";

        if (!(cin >> choice)) {
            cout << "[ERROR] Invalid input!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (choice < 1 || choice > 7) {
            cout << "[ERROR] Please enter a number from 1 to 7.\n";
            continue;
        }

        if (choice == 7) {
            cout << "Exiting Vole Machine Simulator. Goodbye!\n";
            break;
        }

        switch (choice) {
        case 1: {
            string fileName;
            int startAddress;

            cout << "Enter program file name: ";
            cin >> fileName;

            cout << "Enter starting address (decimal): ";
            if (!(cin >> startAddress)) {
                cout << "[ERROR] Invalid address!\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }

            if (machine.loadProgram(fileName, startAddress)) {
                cout << "[SUCCESS] Program loaded successfully.\n";
            }
            else {
                cout << "[ERROR] Failed to load program.\n";
            }
            break;
        }

        case 2: {
            int index;
            int value;

            cout << "Enter register index (0-15): ";
            if (!(cin >> index)) {
                cout << "[ERROR] Invalid register index!\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }

            cout << "Enter value (0-255): ";
            if (!(cin >> value)) {
                cout << "[ERROR] Invalid value!\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }

            if (index < 0 || index > 15 || value < 0 || value > 255) {
                cout << "[ERROR] Register index or value is out of range.\n";
                break;
            }

            machine.setRegister(index, static_cast<uint8_t>(value));
            cout << "[SUCCESS] Register updated.\n";
            break;
        }

        case 3: {
            int address;
            int value;

            cout << "Enter memory address (0-255): ";
            if (!(cin >> address)) {
                cout << "[ERROR] Invalid address!\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }

            cout << "Enter value (0-255): ";
            if (!(cin >> value)) {
                cout << "[ERROR] Invalid value!\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }

            if (address < 0 || address > 255 || value < 0 || value > 255) {
                cout << "[ERROR] Address or value is out of range.\n";
                break;
            }

            machine.setMemory(address, static_cast<uint8_t>(value));
            cout << "[SUCCESS] Memory updated.\n";
            break;
        }

        case 4:
            if (machine.runStep()) {
                cout << "[SUCCESS] One instruction executed.\n";
            }
            else {
                cout << "[INFO] No instruction executed. Check program state.\n";
            }
            break;

        case 5:
            if (machine.runAll()) {
                cout << "[SUCCESS] Program execution completed.\n";
            }
            else {
                cout << "[ERROR] Program execution stopped.\n";
            }
            break;

        case 6:
            machine.display();
            break;
        }
    }
}