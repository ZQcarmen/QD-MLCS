#include "point.h"
#include <iostream>
#include <string>

int PointVHT::size = 0;

PointVHT::PointVHT(int val) {
    coor = new int[size];
    for(int i = 0; i < size; i++) {
        coor[i] = val;
    }
}

void PointGHT::addSuc(int suc) {
    sucs |= (1 << suc);
}
bool PointGHT::hasSuc(int suc) {
    return (sucs & (1 << suc)) == 1;
}

void PointVHT::print(std::string flag) {
    std::cout << flag << ": ";
    for(int i = 0; i < size; i++) {
        std::cout << coor[i] << ' ';
    }
    std::cout << std::endl; 
}

PointVHT::~PointVHT() {
    if(coor) {
        delete[] coor;
    }
}