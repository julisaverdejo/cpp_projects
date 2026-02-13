#include <iostream>
#include "Shape.h"

// Initialize the member variable
Shape::Shape(std::string type) : type(type) {}

//area() is not defined, because is a pure virtual function
//they exist only to define an interface that derived classes must implement

Shape::~Shape(){
    std::cout << type << " is being destroyed." << std::endl;
}