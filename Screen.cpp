#include "Screen.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

Screen::Screen() {
    clear();
}

void Screen::write(uint8_t value) {
    stringstream ss;
    ss << uppercase << hex << setw(2) << setfill('0') << (int)value;
    output += ss.str() + " ";
}

void Screen::clear() {
    output = "";
}

void Screen::display() {
    if (output == "") {
        cout << "No output" << endl;
    }
    else {
        cout << "The output is: " << output << endl;
    }
}

string Screen::get() {
    return output;
}