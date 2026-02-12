#ifndef PERSON_H
#define PERSON_H

#include <string>

class Person {
protected:
    std::string name;
    int age;

public:
    Person(std::string n, int a);
    void introduce();
    virtual ~Person();
};

#endif // PERSON_H
