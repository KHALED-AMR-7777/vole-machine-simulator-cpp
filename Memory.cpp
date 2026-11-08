#include "Memory.h"
#include <iostream>
#include <iomanip>
using namespace std;
using std::uint8_t;

Memory::Memory()
{
    for (int i = 0; i < SIZE; i++)
    {
        cells[i] = 0;
    }
}

bool Memory::isValidAddress(int address) const
{
    if (address >= 0 && address < SIZE)
    {
        return true;
    }
    else
    {
        return false;
    }
}

uint8_t Memory::read(int address) const
{
    if (isValidAddress(address))
    {
        return cells[address];
    }
    else
    {
        cout << "Invalid memory address!" << endl;
        return 0;
    }
}

void Memory::write(int address, uint8_t value)
{
    if (isValidAddress(address))
    {
        cells[address] = value;
    }
    else
    {
        cout << "Invalid memory address!" << endl;
    }
}

void Memory::clear()
{
    for (int i = 0; i < SIZE; i++)
    {
        cells[i] = 0;
    }
}

void Memory::display() const
{
    cout << hex << uppercase << setfill('0');

    cout << "Address    Value" << endl;

    for (int i = 0; i < SIZE; i++)
    {
        cout << setw(2) << i << "         "
            << setw(2) << (int)cells[i] << endl;
    }

    cout << dec;
}