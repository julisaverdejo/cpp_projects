#ifndef SHAPE_H
#define SHAPE_H

#include <string>

class Shape {
protected:
    

public:
    std::string type;
    Shape(std::string type);
    virtual float area() = 0; //Pure virtual function. Each shape calculates its area differently.
    std::string getType() const { return type; }
    virtual ~Shape();
};

#endif
