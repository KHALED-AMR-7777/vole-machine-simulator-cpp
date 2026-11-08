#pragma once
#include <cstdint>
#include "Registers.h"
#include "memory.h"
#include "Screen.h"
using namespace std;

class CPU
{
private:
    Registers registers;

    uint8_t pc;
    uint16_t ir;
    bool halted;

    uint8_t opcode;
    uint8_t r;
    uint8_t x;
    uint8_t y;

    bool fetch(Memory& memory);
    bool decode();
    bool isValid() const;
    bool execute(Memory& memory, Screen& screen);

public:
    CPU();

    void reset(int startAddress);
    bool step(Memory& memory, Screen& screen);
    bool isHalted() const;

    void setRegister(int index, uint8_t value);
    uint8_t getRegister(int index);

    void display();
};
