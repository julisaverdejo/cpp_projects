#include <iostream>
#include "Student.h"

// : Person(n,a) - Constructor initialization list, calls the base class constructor
// before the derived class constructor body runs
Student::Student(std::string n, int a, std::string uni, int id) : Person(n,a) {
    university = uni;
    studentID = id;
}

void Student::study() {
    std::cout << name << " is studying at " << university << std::endl;
}

//Method overriding - lets derived classes provide their own implementation of base class methods
void Student::introduce() {
    std::cout << "Hi, I'm " << name << ", " << age << " years old, ";
    std::cout << "studying at " << university << " (ID: " << studentID << ")" << std::endl;
}

//Virtual destructor in the base class ensures
//proper cleanup when deleting derived objects through base
//class pointers
Student::~Student() {
    std::cout << name << " (Student) is being destroyed." << std::endl;
}
