#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <format>
#include <vector>
#include <cctype>

struct Date {
    int day;
    int month;
    int year;
};

int main(int argc, char* argv[]) {
    Date myDate;

    if (argc < 2) {
        std::cout << "You did not enter a date. Enter one in this format: DD/MM/YYYY" << std::endl;
        return 1;
    }
    
    std::string filedate;
    std::string filename = argv[1];
    std::ifstream InputFile(filename);
    std::ofstream OutputFile("output.txt");

    if (!InputFile.is_open()) {
        std::cout << "[ERROR]: Could not open the file " << filename << std::endl;
        return 1;
    }
    
    std::getline(InputFile, filedate);
    // std::vector<int> date_array;
    // std::string temp;

    // if (filedate.length() == 10) {
    //     for (char c : filedate) {
    //         if (c == '/') {
    //             date_array.push_back(std::stoi(temp));
    //             temp.clear();
    //         } else {
    //             temp += c;
    //         } 
    //     }
    // } else {
    //     std::cout << "You did not enter a valid date" << std::endl;
    //     }

    // for (int v : date_array) {
    //     std::cout << v << std::endl;
    // }   
    myDate.day = std::stoi(filedate.substr(0,2));
    myDate.month = std::stoi(filedate.substr(3,2));
    myDate.year = std::stoi(filedate.substr(6,4));

    auto date = std::chrono::year_month_day{std::chrono::year{myDate.year}, std::chrono::month{unsigned(myDate.month)}, std::chrono::day{unsigned(myDate.day)}};

    if (date.ok()) {
        auto sys_days = std::chrono::sys_days{date};
        auto weekday = std::chrono::weekday{sys_days};
        OutputFile << std::format("{:%A}", weekday) << " " << std::format("{:%d %B, %Y}", date) << std::endl;
    } else {
        OutputFile << "[ERROR]: " << std::format("{:%d %B, %Y}", date) << " is not a valid date" << std::endl;
    }

    InputFile.close();
    OutputFile.close();
    return 0;
}