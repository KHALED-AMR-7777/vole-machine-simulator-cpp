#pragma once
#include <string>
#include <cstdint>
#include "CPU.h"
#include "memory.h"
#include "Screen.h"
#include "Program_Loader.h"
using namespace std;

class Machine
{
private:
    Memory memory;
    CPU cpu;
    Screen screen;
    ProgramLoader loader;

public:
    Machine();

    bool loadProgram(string fileName, int startAddress);
    void setRegister(int index, uint8_t value);
    void setMemory(int address, uint8_t value);
    bool runStep();
    bool runAll();
    void display();
    void reset(int startAddress = 0);
};
