#include "Screen.h"
#include <iostream>
#include <format>

using namespace std;

Screen::Screen(){
    clear();
}


void Screen::write(uint8_t value){
    output += format("{:02X} ",value);
}

void Screen::clear(){
    output="";
}

void Screen::display(){
    if (output == ""){
        cout<< "No output"<< endl;
    }
    else cout<<"The output is: " << output << endl;
}

string Screen::get(){
    return output;
}