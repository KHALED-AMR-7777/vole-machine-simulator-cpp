#include "Menu.h"
#include <iostream>
#include <string>
#include <cstdint>
#include <cctype>

using namespace std;

void Menu::Run() {
    int choice;
    do {
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

        while (!(cin >> choice) || choice < 1 || choice > 7) {
            cout << "[ERROR] Invalid input! Please enter a valid number between 1 and 7: ";
            cin.clear();
            cin.ignore(100000000, '\n');
        }

        switch (choice) {
            case 1: {
                cout << "\n[INFO] Load Program option selected.\n";
                string FillName;
                cout << "Enter the name of the file to load: ";
                cin >> FillName;

                int startAddress;
                cout << "Enter the address of the file to load: ";
                while (!(cin >> startAddress) || startAddress < 0) {
                    cout << "[ERROR] Invalid input! Please enter a valid address: ";
                    cin.clear();
                    cin.ignore(10000000, '\n');
                }

                if (machine.loadProgram(FillName, startAddress)) {
                    cout << "[SUCCESS] Program loaded successfully into memory!\n";
                } else {
                    cout << "[ERROR] Could not open or load the file '" << FillName << "'. Please check the file name and try again.\n";
                }
                break;
            }
            case 2: {
                cout << "\n[INFO] Set Register Value Manually option selected.\n";
                int regIndex;
                string hexValue;

                cout << "Enter register index (0-15): ";
                while (!(cin >> regIndex) || regIndex < 0 || regIndex > 15) {
                    cout << "[ERROR] Invalid register index! Please enter a number between 0 and 15: ";
                    cin.clear();
                    cin.ignore(10000000, '\n');
                }

                bool isValidHex = false;
                while (!isValidHex) {
                    cout << "Enter the 2-digit hex value to set (e.g., A5 or 0f): ";
                    cin >> hexValue;

                    if (hexValue.length() != 2) {
                        cout << "[ERROR] Hex value must be exactly 2 characters long!\n";
                        continue;
                    }

                    isValidHex = true;
                    for (char c : hexValue) {
                        if (!isxdigit(c)) {
                            isValidHex = false;
                            break;
                        }
                    }

                    if (!isValidHex) {
                        cout << "[ERROR] Invalid characters! Please enter valid Hex digits (0-9, A-F).\n";
                    }
                }

                uint8_t value = static_cast<uint8_t>(stoi(hexValue, nullptr, 16));
                machine.setRegister(regIndex, value);
                cout << "[SUCCESS] Register R" << regIndex << " updated successfully with value 0x" << hexValue << "!\n";
                break;
            }
            case 3: {
                cout << "\n[INFO] Set Memory Cell Value Manually option selected.\n";
                int memAddress;
                string hexValue;

                cout << "Enter memory address (positive number): ";
                while (!(cin >> memAddress) || memAddress < 0) {
                    cout << "[ERROR] Invalid memory address! Please enter a valid positive number: ";
                    cin.clear();
                    cin.ignore(10000000, '\n');
                }

                bool isValidHex = false;
                while (!isValidHex) {
                    cout << "Enter the 2-digit hex value for this memory cell (e.g., 7F): ";
                    cin >> hexValue;

                    if (hexValue.length() != 2) {
                        cout << "[ERROR] Hex value must be exactly 2 characters long!\n";
                        continue;
                    }

                    isValidHex = true;
                    for (char c : hexValue) {
                        if (!isxdigit(c)) {
                            isValidHex = false;
                            break;
                        }
                    }

                    if (!isValidHex) {
                        cout << "[ERROR] Invalid characters! Please enter valid Hex digits (0-9, A-F).\n";
                    }
                }

                uint8_t value = static_cast<uint8_t>(stoi(hexValue, nullptr, 16));
                machine.setMemory(memAddress, value);
                cout << "[SUCCESS] Memory cell at address " << memAddress << " updated successfully with 0x" << hexValue << "!\n";
                break;
            }
            case 4: {
                cout << "\n[INFO] Run Step-by-Step option selected.\n";
                if (machine.runStep()) {
                    cout << "[SUCCESS] Executed one step successfully.\n";
                } else {
                    cout << "[INFO] Program halted or execution finished.\n";
                }
                break;
            }
            case 5: {
                cout << "\n[INFO] Run All (Batch Execution) option selected.\n";
                if (machine.runAll()) {
                    cout << "[SUCCESS] Program executed completely.\n";
                } else {
                    cout << "[ERROR] Execution stopped or encountered an issue.\n";
                }
                break;
            }
            case 6: {
                cout << "\n[INFO] Display Machine State option selected.\n";
                machine.display();
                break;
            }
            case 7: {
                cout << "Exiting Vole Machine Simulator. Goodbye!\n";
                break;
            }
        }

    } while (choice != 7);
}