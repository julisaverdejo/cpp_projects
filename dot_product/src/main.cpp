#include <iostream>
#include <fstream>
#include <numeric>
#include <ostream>
#include <vector>
#include "dot_product.h"


int main() {
    std::ifstream InputFile("input.txt");    
    if (!InputFile.is_open()) {
        std::cout << "[ERROR]: Could not open the file " << std::endl;
        return 1;
    }

	std::vector<int> a_vector, b_vector;
    int a,b;
    int result;

    while (InputFile >> a >> b) {
        a_vector.push_back(a);
        b_vector.push_back(b);
    }

    int* arr_a = a_vector.data();
    int* arr_b = b_vector.data();
    int dotprodfun;

    dot_product(arr_a, arr_b, result);
    dotprodfun = std::inner_product(a_vector.begin(), a_vector.end(), b_vector.begin(), 0);

    if(result != dotprodfun) {
        std::cout << "[ERROR]: The dot product is wrong" << std::endl;
    } else {
        std::cout << "The dot product of vectors a and b is: " << result << std::endl;
    }
  InputFile.close();
  return 0;
}