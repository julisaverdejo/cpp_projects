#include <iostream>
#include <fstream>
#include <string>

// argc (ARGument Count) - Integer that stores the number of command line 
//                         arguments passed to the main function.
//                         The name of the program is also included in the count.
// argv (ARGument Vector) - Is a char array pointer. Every element of the array
//                          points to a command line argument, does not store it.
//                          argv[0] is always the program name/path
int main(int argc, char* argv[]) {
// Always check argc before accessing argv. If the program is run without an argument
// argv[1] doesn't exist

    if (argc < 2) {
        std::cout << "The file is empty" << std::endl;
        return 1;
    }

    std::string argmt;
    std::string filename = argv[1];
    std::ifstream InputFile(filename);
    std::ofstream OutputFile("output.txt");
    
    if (!InputFile.is_open()) {
        std::cout << "[ERROR]: Could not open the file " << filename << std::endl;
        return 1;
    }
// getline(stream, string) 
// when stream is used inside a loop or in a boolean context c++ implicity converts it to bool
//  True - if the stream is in a good state
//  False - if the stream encountered an error or reached end-of-line
    int line_number = 0;
    while(std::getline(InputFile, argmt)) {
        OutputFile << "Line " <<  line_number++ << " : " << argmt << std::endl; 
    }

    std::cout << "The file was successfully written!" << std::endl;
    
    InputFile.close();
    OutputFile.close();
    return 0;
}