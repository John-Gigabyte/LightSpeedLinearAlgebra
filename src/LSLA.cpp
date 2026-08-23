#include "LSLA.h"

#include <iostream>
#include <stdexcept>

namespace LSLA {
    void HelloWorld() {
        std::cout << "Hello, World!" << std::endl;
    }

    Vector::Vector(int const size) {
        this->size = size;
        this->data = new float[size];
    }

    Vector::~Vector() {
        delete [] data;
    }

    float &Vector::operator[](const int index) const {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Vector index out of range");
        }
        return data[index];
    }

    void Vector::setData(const std::initializer_list<float> values) const {
        if (static_cast<int>(values.size()) != size) {
            throw std::invalid_argument("Vector::setData: value count does not match vector size");
        }
        int i = 0;
        for (float const value: values) {
            data[i++] = value;
        }
    }

    int Vector::getSize() const {
        return size;
    }

    Vector &Vector::operator=(std::initializer_list<float> const values) {
        setData(values);
        return *this;
    }

    Vector &Vector::operator+(const Vector &other) {
        if (other.size != size) {
            throw std::invalid_argument("Vector size does not match vector size");
        }
        // implement vector + vector addition
        for (int i = 0; i < size; i++) {
            data[i] += other.data[i];
        }
        return *this;
    }

    Vector &Vector::operator+=(const Vector &other) {
        if (other.size != size) throw std::invalid_argument("Vector size does not match vector size");
        for (int i = 0; i < size; i++) {
            data[i] += other.data[i];
        }
        return *this;
    }

    Vector &Vector::operator-(const Vector &other) {
        if (other.size != size) throw std::invalid_argument("Vector size does not match vector size");
        for (int i = 0; i < size; i++) {
            data[i] -= other.data[i];
        }
        return *this;
    }

    Vector &Vector::operator-=(const Vector &other) {
        if (other.size != size) {
            throw std::invalid_argument("Vector size does not match vector size");
        }
        for (int i = 0; i < size; i++) {
            data[i] -= other.data[i];
        }
        return *this;
    }
}
