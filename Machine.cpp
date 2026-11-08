#include "Machine.h"
#include <iostream>
using namespace std;

Machine::Machine()
{
    reset();
}

bool Machine::loadProgram(string fileName, int startAddress)
{
    Memory newMemory;

    if (!loader.loadProgram(fileName, startAddress, newMemory))
        return false;

    memory = newMemory;
    screen.clear();
    cpu.reset(startAddress);
    return true;
}

void Machine::setRegister(int index, uint8_t value)
{
    cpu.setRegister(index, value);
}

void Machine::setMemory(int address, uint8_t value)
{
    if (address == 255)
        screen.write(value);
    else
        memory.write(address, value);
}

bool Machine::runStep()
{
    return cpu.step(memory, screen);
}

bool Machine::runAll()
{
    int stepLimit = 10000;
    int stepsExecuted = 0;
    
    while (!cpu.isHalted())
    {
        if (!cpu.step(memory, screen))
            return false;

        stepsExecuted++;
        
        if (stepsExecuted >= stepLimit)
        {
            cout << "Error: Execution aborted. Infinite loop detected (exceeded " << stepLimit << " steps)." << endl;
            return false;
        }    
    }

    return true;
}

void Machine::display()
{
    cout << "\n========== MACHINE STATE ==========" << endl;
    cout << "\nCPU and Registers:" << endl;
    cpu.display();
    cout << "\nMemory:" << endl;
    memory.display();
    cout << "\nScreen:" << endl;
    screen.display();
    cout << "===================================" << endl;
}

void Machine::reset(int startAddress)
{
    memory.clear();
    cpu.reset(startAddress);
    screen.clear();
}

uint8_t Machine::getRegister(int index){
    return cpu.getRegister(index);
}

uint8_t Machine::getMemory(int address) const {
    return memory.read(address);
}

int Machine::getPC() const {
    return cpu.getPC();
}

int Machine::getIR() const {
    return cpu.getIR();
}

std::string Machine::getScreenOutput(){
    return screen.get();
}

bool Machine::isHalted() const {
    return cpu.isHalted();
}