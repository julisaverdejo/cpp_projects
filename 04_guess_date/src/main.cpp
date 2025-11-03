#include <iostream>
#include <chrono>
#include <string>
#include <iomanip>

struct Date {
    int day;
    int month;
    int year;
};


int main() {
    char slash1, slash2;
    Date myDate;

    // auto epoch = std::chrono::system_clock::time_point{};
    // std::cout << epoch << std::endl;

    std::cout << "Enter a date (DD/MM/YYYY), and I will tell you the weekday" << std::endl;
    std::cin >> myDate.day >> slash1 >> myDate.month >> slash2 >> myDate.year;

    auto date = std::chrono::year_month_day{std::chrono::year{myDate.year}, std::chrono::month{unsigned(myDate.month)}, std::chrono::day{unsigned(myDate.day)}};
    std::cout << "The date you enter is: " << std::format("{:%B %d, %Y}", date) << std::endl;

    if (date.ok()){
        auto sys_days = std::chrono::sys_days{date};
        //std::cout << sys_days << std::endl;
        auto weekday = std::chrono::weekday{sys_days};
        std::cout << "The weekday is: " << std::format("{:%A}",weekday) << std::endl;            
    } else {
        std::cout << "The date you enter is not valid" << std::endl;            
    }
         
    return 0;
}