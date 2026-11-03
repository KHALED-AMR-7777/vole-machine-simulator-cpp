#pragma once
#include <cstdint>

class Registers {
private:
    static  const int SIZE = 16;
    uint8_t regs[SIZE];
public:
    Registers();
    void set(int index, uint8_t value);
    uint8_t get(int index);
    void clear();
    void display();
};