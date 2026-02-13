#ifndef SQUARE_H
#define SQUARE_H

#include "Shape.h"

//Derived class, inherit all members from the base class and can add their own
class Rectangle : public Shape {
private:
    float length;
    float width;

public:
    Rectangle(float l, float w);
    float area() override;
    void checktype();
    ~Rectangle();    
};

#endif