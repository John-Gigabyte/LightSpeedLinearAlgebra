#include "LSLA.h"

#include <iostream>
#include <stdexcept>

namespace LSLA {
    void HelloWorld() {
        std::cout << "Hello, World!" << std::endl;
    }
    Vector::Vector()
    {
        this->size = 0;
        this->data = new float[3];
    }
    Vector::Vector(int const size) {
        this->size = size;
        this->data = new float[size];
    }

    Vector::Vector(const Vector &other) {
        size = other.size;
        data = new float[size];
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }

    Vector &Vector::operator=(const Vector &other) {
        if (this == &other) {
            return *this;
        }
        delete [] data;
        size = other.size;
        data = new float[size];
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
        return *this;
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

    float Vector::operator*(const Vector &other) const
    {
        if (other.size != size)
        {
            throw std::invalid_argument("Vector size does not match vector size");
        }
        float product = 0.0f;
        for (int i = 0; i < size; i++)
        {
            product += data[i] * other.data[i];
        }
        return product;
    }

    Vector Vector::operator^(const Vector &other) const
    {
        if (other.size != size) throw std::invalid_argument("Vector size does not match vector size");
        if (size != 3)
        {
            throw std::invalid_argument("Cross Products can only be calculated on 3 dimensions");
        }
        Vector cross(3);
        cross.data[0] = data[1] * other.data[2] - data[2] * other.data[1];
        cross.data[1] = data[2] * other.data[0] - data[0] * other.data[2];
        cross.data[2] = data[0] * other.data[1] - data[1] * other.data[0];
        return cross;
    }
}
