#pragma once

#include <cstdint>
#include <string>

class Screen {
private:
    std::string output;

public:
    Screen();

    void write(uint8_t value);
    void clear();
    void display();
    std::string get();
};