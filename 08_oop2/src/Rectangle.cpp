#include <iostream>
#include "Rectangle.h"

Rectangle::Rectangle(float l, float w) : Shape("Rectangle"), length(l), width(w) {}


//Method overriding - lets derived classes provide their own implementation of base class methods
// Non-void function (int, double, float, std::string) MUST use return with a value.
float Rectangle::area() {
    float result = (length * width);
    return result;
}

void Rectangle::checktype() {
    if (length == width) {
        std::cout << "It's a square" << std::endl;
    } else {
        std::cout << "It's a rectangle" << std::endl;
    }
}

Rectangle::~Rectangle() {
    std::cout << "Square out" << std::endl;
}