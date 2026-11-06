#pragma once
#include <string>
#include "Memory.h"

class ProgramLoader
{
public:
    bool loadProgram(std::string fileName, int startAddress, Memory& memory);
};