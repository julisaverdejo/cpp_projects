#ifndef ANIMAL_H
#define ANIMAL_H


//Blueprint class
// An abstract class - at least has one pure virtual function
// any class that inherits from it must implement ALL pure virtual functions
// otherwise the derived class is also abstract
class Animal {
public:
    virtual void speak() = 0;
};

#endif