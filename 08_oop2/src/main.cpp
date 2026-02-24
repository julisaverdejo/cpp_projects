#include <iostream>
#include "Triangle.h"
#include "Rectangle.h"

int main() {
	// Static creation - object on the stack
	Triangle triangle1(10, 7);
	triangle1.area();
    std::cout << "The area of the triangle is " << triangle1.area() << std::endl;

	//Dynamic creation - object on the heap
	//Accessing to type using the getter method
	Triangle* triangle2 = new Triangle(3,3);
    std::cout << "The area of the " << triangle2->getType() << " is " << triangle2->area() << std::endl;

    Rectangle* rec1 = new Rectangle(5,10);
	rec1->checktype();
	std::cout << "The area is " << rec1->area() << std::endl;

	Shape* shape =  new Rectangle(2,2);
	std::cout << shape->area() << std::endl;
	std::cout << shape->getType() << std::endl;

	Shape* arr[3];
	arr[0] = &triangle1;
	arr[1] = triangle2;
	arr[2] = rec1;

	int j = 0;
	for (Shape* i : arr) {
		std::cout << "Iter " << j << " : " << i->area() << std::endl;
		j++;
	}

	std::cout << rec1 << "\n";
	std::cout << &(rec1->type) << "\n";
	std::cout << &(rec1->length) << "\n";

    // Cleanup
	delete triangle2;
	delete rec1;


	return 0;
}

