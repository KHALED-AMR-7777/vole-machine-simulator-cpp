#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <format>

using namespace std;


class Registers {
    static  const int SIZE = 16;
    uint8_t regs[SIZE];
public:
    void set(int index, uint8_t value){
        
        if (index>=0 && index<SIZE){
            regs[index]=value;
        }
        else throw out_of_range("Index is out of bounds!");
    }


    uint8_t get(int index){
        if (index>=0 && index<SIZE){
            return regs[index];
        }
        else throw out_of_range("Index is out of bounds!");
    }
    void clear(){
        for (int i=0; i<SIZE; i++){
            regs[i]=0;
        }
    }
    void display(){
        for (int i = 0; i < SIZE; i++){
        string hex_output = format("R{:X}: {:02X}", regs[i]); //Needs c++ 20 if you got an error. It needs to be added in README too
        cout << hex_output << endl;
        }
    }
    Registers(){
        clear();
    }
};

int main() {
    return 0;
}
