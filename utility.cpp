#include "utility.hpp"

#include <iostream>

void printSectionDescription(const std::string &name, std::size_t size)
{
    std::cout << "Segment started: " << name << std::endl;
    std::cout << "Segment size: " << size << " bytes" << std::endl;
}
