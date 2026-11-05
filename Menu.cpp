//
// Created by DELL on 11/5/2026.
//

#include "Menu.h"
#include <iostream>
#include<bits/stdc++.h>
using namespace std;


void Menu::Run() {
    int choice;
    // the menu
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

    // her if you cin value not a number or number > 7
    while (!(cin >> choice) or choice > 7) {
        cout << "[ERROR] Invalid input! Please enter a valid number.\n";
        cin.clear();
        cin.ignore(100000000,'\n');
    }
    // switch case for input valdition
    switch (choice) {
        case 1:
            cout << "\n[INFO] Load Program option selected.\n";
            break;

        case 2:
            cout << "\n[INFO] Set Register option selected.\n";
            break;

        case 3:
            cout << "\n[INFO] Set Memory option selected.\n";
            break;

        case 4:
            cout << "\n[INFO] Run Step-by-Step selected.\n";
            break;

        case 5:
            cout << "\n[INFO] Run All selected.\n";
            break;

        case 6:
            cout << "\n[INFO] Display State selected.\n";
            break;

        case 7:
            cout << "Exiting Vole Machine Simulator. Goodbye!\n";
            break;

    }
}