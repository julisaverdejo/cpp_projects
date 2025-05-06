#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

int main(){
    std::vector<int> column_1;
    std::vector<int> column_2;
    
    std::ofstream outFile("new_file.txt"); // Open or create a file
    if (!outFile) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }

    for(int i = 0; i < 10; i++){
       column_1.push_back(i); // Adds i values to the vector
       column_2.push_back(i+1);
       outFile << column_1[i] << "\t " << column_2[i] << std::endl;
    }


    outFile.close();
    return 0;
}