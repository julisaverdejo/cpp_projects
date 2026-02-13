#include <iostream>
#include "Triangle.h"

int main() {
	// Static creation - object on the stack
	Triangle triangle1(10, 7);
	triangle1.area();
    std::cout << "The area of the triangle is " << triangle1.area() << std::endl;


	return 0;
}
