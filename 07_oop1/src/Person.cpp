#include <iostream>
#include "Person.h"

Person::Person(std::string n, int a){
    name = n;
    age = a;
}

void Person::introduce(){
    std::cout << "Hi, I'm " << name << " and I'm " << age << " years old." << std::endl;
}

Person::~Person(){
    std::cout << name << " (Person) is being destroyed." << std::endl;
}