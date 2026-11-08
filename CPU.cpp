#include "CPU.h"
#include <iostream>
#include <iomanip>
#include <cmath>
using namespace std;

CPU::CPU()
{
    reset(0);
}

void CPU::reset(int startAddress)
{
    registers.clear();
    pc = (uint8_t)startAddress;
    ir = 0;
    halted = false;
    opcode = 0;
    r = 0;
    x = 0;
    y = 0;
}

bool CPU::fetch(Memory& memory)
{
    if (pc >= 255)
    {
        cout << "Error: Program counter is outside program memory!" << endl;
        halted = true;
        return false;
    }

    uint8_t firstByte = memory.read(pc);
    uint8_t secondByte = memory.read(pc + 1);

    ir = ((uint16_t)firstByte << 8) | secondByte;
    pc += 2;
    return true;
}

bool CPU::decode()
{
    opcode = (ir >> 12) & 0x0F;
    r = (ir >> 8) & 0x0F;
    x = (ir >> 4) & 0x0F;
    y = ir & 0x0F;
    return true;
}

bool CPU::isValid() const
{
    if (opcode >= 1 && opcode <= 3)
        return true;

    if (opcode == 4)
        return r == 0;

    if (opcode >= 5 && opcode <= 9)
        return true;

    if (opcode == 10)
        return x == 0;

    if (opcode == 11)
        return true;

    if (opcode == 12)
        return r == 0 && x == 0 && y == 0;

    return false;
}

static float convertToFloat(uint8_t value)
{
    int sign = (value >> 7) & 1;
    int exponent = (value >> 4) & 0x07;
    int mantissa = value & 0x0F;

    float number = (float)mantissa / 16.0f;
    number *= pow(2.0f, exponent - 4);

    if (sign == 1)
        number *= -1;

    return number;
}

static uint8_t convertFromFloat(float number)
{
    if (number == 0)
        return 0;

    int sign = number < 0 ? 1 : 0;
    number = fabs(number);

    int exponent = 0;
    while (number >= 1.0f && exponent < 3)
    {
        number /= 2.0f;
        exponent++;
    }
    while (number < 0.5f && exponent > -4)
    {
        number *= 2.0f;
        exponent--;
    }

    int storedExponent = exponent + 4;
    if (storedExponent < 0)
        storedExponent = 0;
    if (storedExponent > 7)
        storedExponent = 7;

    int mantissa = (int)(number * 16.0f + 0.5f);
    if (mantissa > 15)
        mantissa = 15;
    if (mantissa < 0)
        mantissa = 0;

    return (uint8_t)((sign << 7) | (storedExponent << 4) | mantissa);
}

bool CPU::execute(Memory& memory, Screen& screen)
{
    int address = x * 16 + y;

    switch (opcode)
    {
    case 1:
        registers.set(r, memory.read(address));
        break;

    case 2:
        registers.set(r, (uint8_t)address);
        break;

    case 3:
        if (address == 255)
            screen.write(registers.get(r));
        else
            memory.write(address, registers.get(r));
        break;

    case 4:
        registers.set(y, registers.get(x));
        break;

    case 5:
        registers.set(r, (uint8_t)(registers.get(x) + registers.get(y)));
        break;

    case 6:
    {
        float first = convertToFloat(registers.get(x));
        float second = convertToFloat(registers.get(y));
        registers.set(r, convertFromFloat(first + second));
        break;
    }

    case 7:
        registers.set(r, registers.get(x) | registers.get(y));
        break;

    case 8:
        registers.set(r, registers.get(x) & registers.get(y));
        break;

    case 9:
        registers.set(r, registers.get(x) ^ registers.get(y));
        break;

    case 10:
    {
        uint8_t value = registers.get(r);
        for (int i = 0; i < y; i++)
        {
            uint8_t lastBit = value & 1;
            value = (value >> 1) | (lastBit << 7);
        }
        registers.set(r, value);
        break;
    }

    case 11:
        if (registers.get(r) == registers.get(0))
            pc = (uint8_t)address;
        break;

    case 12:
        halted = true;
        break;

    default:
        return false;
    }

    return true;
}

bool CPU::step(Memory& memory, Screen& screen)
{
    if (halted)
        return false;

    if (!fetch(memory))
        return false;

    decode();

    if (!isValid())
    {
        cout << "Error: Invalid instruction!" << endl;
        halted = true;
        return false;
    }

    if (!execute(memory, screen))
    {
        cout << "Error: Could not execute instruction!" << endl;
        halted = true;
        return false;
    }

    return true;
}

bool CPU::isHalted() const
{
    return halted;
}

void CPU::setRegister(int index, uint8_t value)
{
    registers.set(index, value);
}

uint8_t CPU::getRegister(int index)
{
    return registers.get(index);
}

void CPU::display()
{
    cout << "PC: " << hex << uppercase << (int)pc << endl;
    cout << "IR: " << setw(4) << setfill('0') << ir << endl;
    cout << dec;
    registers.display();
}
