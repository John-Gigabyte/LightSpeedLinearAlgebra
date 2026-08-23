#include "LSLA.h"
#include <iostream>

int main() {
    LSLA::HelloWorld();
    LSLA::Vector vector(5);
    vector.setData({1, 2, 3, 4, 5});
    std::cout << vector[3] << std::endl;

    vector = {5, 4, 3, 2, 1};
    std::cout << vector[3] << std::endl;

    return 0;
}