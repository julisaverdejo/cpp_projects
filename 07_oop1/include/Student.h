#ifndef STUDENT_H
#define STUDENT_H

#include "Person.h"

//Derived class, inherit all members from the base class and can add their own
class Student : public Person {
private:
    std::string university;
    int studentID;

public:
    Student(std::string n, int a, std::string uni, int id);
    void study();
    void introduce();
    ~Student();    
};

#endif