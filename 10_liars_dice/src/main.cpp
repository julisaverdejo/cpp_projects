#include <iostream>
#include "Dog.h"
#include "Cat.h"
#include "Zoo.h"

int main() {

    //created on the heap	
	// dog, cat and zoo are pointers

	// new Dog() - creates the object on the heap, returns a memory address
	// Dog* dog = - stores that address in the pointer	
    Dog* dog = new Dog();
	Cat* cat = new Cat();
	Zoo* zoo = new Zoo();

    //Use -> to access members 
	zoo->makeItSpeak(dog);
	zoo->makeItSpeak(cat);

	delete dog;
	delete cat;
	delete zoo;


	return 0;
}

