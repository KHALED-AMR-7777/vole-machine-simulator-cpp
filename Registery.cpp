#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <format>
#include "Registers.h"
using namespace std;

Registers::Registers() {
    clear();
}

void Registers::set(int index, uint8_t value){

if (index>=0 && index<SIZE){
    regs[index]=value;
    }
else throw out_of_range("Index is out of bounds!");

}


uint8_t Registers::get(int index){
    if (index>=0 && index<SIZE){
        return regs[index];
    }
    else throw out_of_range("Index is out of bounds!");
}

void Registers::clear(){
    for (int i=0; i<SIZE; i++){
        regs[i]=0;
    }
}
void Registers::display(){
    for (int i = 0; i < SIZE; i++){
        string hex_output = format("R{:X}: {:02X}", i,regs[i]); //Needs c++ 20 if you got an error. It needs to be added in README too
        cout << hex_output << endl;
    }
}
