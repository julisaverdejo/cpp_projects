#include <iostream>
#include "Student.h"

int main() {

	//Static object creation (on the stack)
	//automatically destroyed when they go out of scope
	Student student1("Ciro", 29, "BUAP", 12345);
	student1.introduce();
	student1.study();

	std::cout << std::endl;

    //Polymorphism - base class pointer to derived class object
	Person* person = new Student("Julisa", 28, "BUAP", 201510277);
	//Access members through a pointer
	person->introduce();

    //Always delete dynamically allocated objects
	delete person;

	return 0;
}
