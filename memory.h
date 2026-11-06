#pragma once
#include <cstdint>

class Memory {
private:
    static const int SIZE = 256;
    uint8_t cells[SIZE];

    void validateAddress(int address) const;

public:
    Memory();

    uint8_t read(int address) const;
    void write(int address, uint8_t value);
    void clear();
    void display() const;
};