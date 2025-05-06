#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <iomanip>

int main(){
    const double step_size = 0.01;
    std::vector<double> y;

    std::ofstream outFile("output.txt"); // Open or create a file
    if (!outFile) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }
    // outFile << std::setw(7) << "x" << std::setw(9) << "f(x)" << "\n";
    for (double x = 0; x <= 10; x += step_size){
        double value = std::sin(x) + std::cos(x) + 4.7*std::sin(3.3*x) + x*x;
        y.push_back(value);
        outFile << std::setw(7) << std::fixed << std::setprecision(2) << x << ", " 
                << std::setw(7) << std::fixed << std::setprecision(2) << value << "\n";      
    }

    outFile.close();
    return 0;
}