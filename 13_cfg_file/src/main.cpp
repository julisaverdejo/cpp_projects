#include <iostream>
#include "cfg.h"

int main()
{
    CFG_STRUCT(Student,
        CFG_ITEM(std::string, name, "Name");
        CFG_ITEM(int, ID, 0);
        CFG_ITEM(int, age, 0);
        CFG_ITEM(bool, aproved, false);
    );

    CFG_STRUCT(Classroom,
        CFG_ARRAY(Student, 4, student);
    );

    Classroom config{"Classroom"};
    if (!config.readFile("test.cfg")) {
        std::cerr << "Failed to read configuration\n";
        return 1;
    }
    // We can access or change the value of the struct
    // elements like normal variables
    // int age = config.student[2].age;
    // bool enabled = config.channels[2].enabled;
    std::string name = config.student[2].name;
    
    std::cout << "The ID of " << name << " is " << config.student[2].ID << "\n";
    // std::cout << "Enabled" << enabled << "\n";

    config.writeFile("output.txt");
}