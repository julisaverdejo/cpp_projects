#include <iostream>
#include "Zoo.h"


//makeItSpeak calls the speak() of the concrete class
//concrete class - derived class of Animal
void Zoo::makeItSpeak(Animal* animal) {
    animal->speak();
}