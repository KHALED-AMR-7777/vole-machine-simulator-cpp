#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "Registers.h"

using namespace std;

Registers::Registers() {
    clear();
}

void Registers::set(int index, uint8_t value) {
    if (index >= 0 && index < SIZE) {
        regs[index] = value;
    }
    else {
        throw out_of_range("Index is out of bounds!");
    }
}

uint8_t Registers::get(int index) {
    if (index >= 0 && index < SIZE) {
        return regs[index];
    }
    else {
        throw out_of_range("Index is out of bounds!");
    }
}

void Registers::clear() {
    for (int i = 0; i < SIZE; i++) {
        regs[i] = 0;
    }
}

void Registers::display() {
    for (int i = 0; i < SIZE; i++) {
        stringstream ss;

        ss << "R" << uppercase << hex << i << ": "
           << setw(2) << setfill('0') << (int)regs[i];

        cout << ss.str() << endl;
    }
}