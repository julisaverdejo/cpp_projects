#ifndef SHAPE_H
#define SHAPE_H

#include <string>

class Shape {
protected:
    std::string type;

public:
    Shape(std::string type);
    virtual float area() = 0; //Pure virtual function. Each shape calculates its area differently.
    virtual ~Shape();
};

#endif
