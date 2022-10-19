#include "package.hpp"

#include <iostream>
#include <cstring>
#include <vector>

int main()
{
    const char* name = "marry";
    auto cal1 = Calculator<char*(char*, const char*)>::init(strcpy);
    auto cal2 = Calculator<size_t(const char*)>::init(strlen);
    char *des = new char[cal2(name) + 1];
    cal1(des, name);

    std::cout << des << " == " << name << std::endl;
}