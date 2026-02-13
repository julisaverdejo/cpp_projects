#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Shape.h"

//Derived class, inherit all members from the base class and can add their own
class Triangle : public Shape {
private:
    float base;
    float height;

public:
    Triangle(float b, float h);
    float area() override;
    ~Triangle();    
};

#endif