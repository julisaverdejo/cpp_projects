#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <iomanip>

double roundToN(double value, int decimals){
    const double multiplier = std::pow(10,decimals);
    return std::round(value*multiplier)/multiplier;
}

int main(){
    const double step_size = 0.1;
    std::vector<double> y;

    std::ofstream outFile("output.txt"); // Open or create a file
    if (!outFile) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }
    outFile << std::setw(5) << "x" << std::setw(10) << "f(x)\n";
    for (double x = 0; x <= 10; x += step_size){
        double value = roundToN((std::sin(x) + std::cos(x)), 4);
        y.push_back(value);
        outFile << std::setw(5) << x << std::setw(10) << value << "\n";      
    }

    outFile.close();
    return 0;
}