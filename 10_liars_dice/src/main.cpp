#include <iostream>
#include "Dice.h"


int main() {
	Dice* myDice = new Dice();
	myDice->display()	;
	myDice->roll();
	myDice->display();

	std::cout << "Value: " << myDice->getValue() << std::endl;

	delete myDice;
	return 0;
}

