#include <iostream>
#include "Triangle.h"

Triangle::Triangle(float b, float h) : Shape("Triangle"), base(b), height(h) {}


//Method overriding - lets derived classes provide their own implementation of base class methods
// Non-void function (int, double, float, std::string) MUST use return with a value.
float Triangle::area() {
    float result = (base * height) / 2.0;
    return result;
}

Triangle::~Triangle() {
    std::cout << "Destroyed." << std::endl;
}