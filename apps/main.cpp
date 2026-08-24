#include "LSLA.h"
#include <iostream>

int main() {
    LSLA::HelloWorld();
    LSLA::Vector vector(5);
    LSLA::Vector vector2(5);
    vector.setData({1, 2, 3, 4, 5});
    std::cout << vector[3] << std::endl;

    vector2 = {5, 4, 3, 2, 1};
    std::cout << vector2[3] << std::endl;

    LSLA::Vector vector3 = vector + vector2;
    std::cout << vector3[3] << std::endl;

    return 0;
}