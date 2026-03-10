#include <iostream>
#include <cstdlib>
#include <ctime>
#include <random>
#include "Dice.h"


Dice::Dice() {
    faceValue = 1;
}

void Dice::roll() {
    // generates a random seed (based on hardware noise)
    std::random_device rd;
    // the engine that generates numbers
    std::mt19937 gen(rd());
    //guarantees every number in the range has exactly the same probability of appearing
    std::uniform_int_distribution<> dist(1,6);
    // pulls numbers from the engine and maps them to your range
    faceValue = dist(gen);
}

int Dice::getValue() const {
    return faceValue;
}

void Dice::display() const {
    std::cout << "[" << faceValue << "]" << std::endl;
}

