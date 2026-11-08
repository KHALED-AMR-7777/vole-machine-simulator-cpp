#ifndef VOLE_MACHINE_SIMULATOR_CPP_MENU_H
#define VOLE_MACHINE_SIMULATOR_CPP_MENU_H

#include <iostream>
#include <string>
#include "Machine.h"

using namespace std;

class Menu {
private:
    Machine machine;

public:
    void Run();
};

#endif