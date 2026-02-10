#include <iostream>
#include "fun.h"
#include "fun2.h"

int main() {
	for (int i = 0; i < 10; i++){
	  std::cout << i << ": Hello" << std::endl;
	}

	print_my_name();
	print_address();
  
  return 0;
}
