#include <iostream>
#include "Triangle.h"

int main() {
	// Static creation - object on the stack
	Triangle triangle1(10, 7);
	triangle1.area();
    std::cout << "The area of the triangle is " << triangle1.area() << std::endl;

	//Dynamic creation - object on the heap
	Triangle* triangle2 = new Triangle(3,3);
    std::cout << "The area of the triangle is " << triangle2->area() << std::endl;

    // Cleanup
	delete triangle2;


	return 0;
}
