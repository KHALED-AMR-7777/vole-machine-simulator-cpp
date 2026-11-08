#pragma once

#include <string>
#include <cstdint>

#include "CPU.h"
#include "Memory.h"
#include "Screen.h"
#include "Program_Loader.h"

class Machine {
private:
    Memory memory;
    CPU cpu;
    Screen screen;
    ProgramLoader loader;

public:
    Machine();

    bool loadProgram(std::string fileName, int startAddress);
    void setRegister(int index, uint8_t value);
    void setMemory(int address, uint8_t value);
    bool runStep();
    bool runAll();
    void display();
    void reset(int startAddress = 0);
};