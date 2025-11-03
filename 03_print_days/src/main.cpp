#include <iostream>
#include <chrono>
#include <format>
#include <iomanip>

// Directive that allows you to avoid typing the namespace prefix
using namespace std::chrono;
int main() {
    int i = 0;

    // Fixed date
    // year_month_day date{2024y/February/29d};

    // Current date
    // system_clock class - represents the system-wide real time YY-MM-DD HH:MM:SS:MSMS
    // now() function - returns the current time in Coordinated Universal Time (UTC), not your local timezone
    auto now = system_clock::now() + hours(1);
    auto now_short = floor<seconds>(now);
    
    // Truncated date keeps the date not the HH:MM:SS:MSMS
    auto date = floor<days>(now);
    // year_month_day class - represents a specific year, month, and day. You can access to each of those objects,
    //                        and do arithmetic operations.
    // start_date - stores the date as YY-MM-DD
    year_month_day start_date{date};
    // weekday class - represent a day of the week
    // Extracts the weekday
    weekday wd{date};

    /* std::chrono::day x{1};
    std::cout << "Hello, world!" << std::endl;
    std::cout << "The number in hex is: " << std::hex << 42 << std::endl;
    std::cout << "Today is: " << now << std::endl;     
    std::cout << "Today is: " << floored_to_days << std::endl;
    std::cout << "year: "  << start_date.year()  << ", "
                 "month: " << start_date.month() << ", "
                 "day: "   << start_date.day()   << std::endl; */ 
    //std::cout << wd++ << std::endl;
    std::cout << std::left << std::setw(10) << "CURRENT DATE: " << std::format("{:%A}", wd) << ", " << std::format("{:%B %d, %Y %H:%M:%S}", now_short) << std::endl;
    std::cout << "The next seven days are: " << std::endl;

    while (i < 7) {
        i++;
        // sys_days - converts start_date as a number, the date is a starting point so it's represented with the number 0.
        //            You can do arithmetic operations. You can add days, months or years.
        //            Adding days gives a new date
        // Then new date is converted to the YY-MM-DD format
        // days - typedef use for do automatic conversions, arithmetic operations.
        year_month_day new_date = sys_days{start_date} + days{i};

        // Use in case you want to use a fixed date
        // year_month_day next_day = sys_days{date} + days{i};
        
        // std::left - Left-aligns the text (adds spaces on the right)
        // std::setw(X) - Sets the field width X characters
        // : - tells std::format that what follows is a format specification
        // %A - write's the locale's full weekday name
        // %B - write's the locale's full month name
        // %d - writes the day of month as a decimal number
        // %Y - writes the year as a decimal number
        std::cout << std::left << std::setw(10) << std::format("{:%A}", wd++) << ", " << std::format("{:%B %d, %Y}", new_date) << std::endl;
    }
                             
    return 0;
}
