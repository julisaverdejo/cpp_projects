#ifndef SQUARE_H
#define SQUARE_H

#include "Shape.h"

//Derived class, inherit all members from the base class and can add their own
class Rectangle : public Shape {
private:
    float width;

public:
    float length;
    Rectangle(float l, float w);
    float area() override;
    float getLength() const { return length; }
    void checktype();
    ~Rectangle();    
};

#endif