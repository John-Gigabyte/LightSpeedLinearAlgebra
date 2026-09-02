//
// Created by johng on 8/30/26.
//
#include "LSLASimple.h"

#include <cmath>
#include <iostream>
#include <stdexcept>

namespace LSLASimple {
    void HelloWorld() {
        std::cout << "Hello, World!" << std::endl;
    }

    // ---------- Construction / destruction ----------

    Vector::Vector() : size(0), data(nullptr) {}

    Vector::Vector(const int size) : size(size), data(nullptr) {
        if (size < 0) {
            throw std::invalid_argument("Vector size cannot be negative");
        }

        data = size > 0 ? new float[size]() : nullptr;
    }

    Vector::Vector(const std::initializer_list<float> values)
        : size(static_cast<int>(values.size())), data(nullptr) {
        data = size > 0 ? new float[size] : nullptr;
        int i = 0;
        for (const float value : values) {
            data[i++] = value;
        }
    }

    Vector::Vector(const Vector &other) : size(other.size), data(nullptr) {
        data = size > 0 ? new float[size] : nullptr;
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }

    Vector &Vector::operator=(const Vector &other) {
        if (this == &other) {
            return *this;
        }
        float *newData = other.size > 0 ? new float[other.size] : nullptr;
        for (int i = 0; i < other.size; i++) {
            newData[i] = other.data[i];
        }
        delete[] data;
        data = newData;
        size = other.size;
        return *this;
    }


    Vector::Vector(Vector &&other) noexcept : size(other.size), data(other.data) {
        other.size = 0;
        other.data = nullptr;
    }

    Vector &Vector::operator=(Vector &&other) noexcept {
        if (this == &other) {
            return *this;
        }
        delete[] data;
        size = other.size;
        data = other.data;
        other.size = 0;
        other.data = nullptr;
        return *this;
    }

    Vector::~Vector() {
        delete[] data;
    }

    // ---------- Element access ----------

    float &Vector::operator[](const int index) {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Vector index out of range");
        }
        return data[index];
    }

    const float &Vector::operator[](const int index) const {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Vector index out of range");
        }
        return data[index];
    }

    int Vector::getSize() const {
        return size;
    }


    Vector &Vector::operator=(const std::initializer_list<float> values) {
        const int count = static_cast<int>(values.size());
        float *newData = count > 0 ? new float[count] : nullptr;
        int i = 0;
        for (const float value : values) {
            newData[i++] = value;
        }
        delete[] data;
        data = newData;
        size = count;
        return *this;
    }

    // ---------- Vector <-> Vector ----------

    Vector Vector::operator+(const Vector &other) const {
        Vector result(*this);
        result += other;
        return result;
    }

    Vector &Vector::operator+=(const Vector &other) {
        if (other.size != size) {
            throw std::invalid_argument("Vector size does not match vector size");
        }
        for (int i = 0; i < size; i++) {
            data[i] += other.data[i];
        }
        return *this;
    }

    Vector Vector::operator-(const Vector &other) const {
        Vector result(*this);
        result -= other;
        return result;
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

    float Vector::operator*(const Vector &other) const {
        if (other.size != size) {
            throw std::invalid_argument("Vector size does not match vector size");
        }
        float product = 0.0f;
        for (int i = 0; i < size; i++) {
            product += data[i] * other.data[i];
        }
        return product;
    }

    Vector Vector::operator^(const Vector &other) const {
        if (other.size != size) {
            throw std::invalid_argument("Vector size does not match vector size");
        }
        if (size != 3) {
            throw std::invalid_argument("Cross products can only be calculated on 3 dimensions");
        }
        Vector cross(3);
        cross.data[0] = data[1] * other.data[2] - data[2] * other.data[1];
        cross.data[1] = data[2] * other.data[0] - data[0] * other.data[2];
        cross.data[2] = data[0] * other.data[1] - data[1] * other.data[0];
        return cross;
    }

    // ---------- Vector <-> scalar ----------

    Vector Vector::operator*(const float scalar) const {
        Vector result(*this);
        result *= scalar;
        return result;
    }

    Vector &Vector::operator*=(const float scalar) {
        for (int i = 0; i < size; i++) {
            data[i] *= scalar;
        }
        return *this;
    }

    Vector Vector::operator/(const float scalar) const {
        Vector result(*this);
        result /= scalar;
        return result;
    }

    Vector &Vector::operator/=(const float scalar) {
        if (scalar == 0.0f) {
            throw std::invalid_argument("Division by zero");
        }
        for (int i = 0; i < size; i++) {
            data[i] /= scalar;
        }
        return *this;
    }

    Vector Vector::operator+(const float scalar) const {
        Vector result(*this);
        result += scalar;
        return result;
    }

    Vector &Vector::operator+=(const float scalar) {
        for (int i = 0; i < size; i++) {
            data[i] += scalar;
        }
        return *this;
    }

    Vector Vector::operator-(const float scalar) const {
        Vector result(*this);
        result -= scalar;
        return result;
    }

    Vector &Vector::operator-=(const float scalar) {
        for (int i = 0; i < size; i++) {
            data[i] -= scalar;
        }
        return *this;
    }

    Vector Vector::power(const float scalar) const {
        Vector result(size);
        for (int i = 0; i < size; i++) {
            result.data[i] = std::pow(data[i], scalar);
        }
        return result;
    }

    // ---------- Unary and comparison ----------
    Vector Vector::operator-() const {
        Vector result(size);
        for (int i = 0; i < size; i++) {
            result.data[i] = -data[i];
        }
        return result;
    }

    bool Vector::operator==(const Vector &other) const {
        if (size != other.size) {
            return false;
        }
        for (int i = 0; i < size; i++) {
            if (data[i] != other.data[i]) {
                return false;
            }
        }
        return true;
    }

    bool Vector::operator!=(const Vector &other) const {
        return !(*this == other);
    }

    // ---------- Free functions (scalar on the left) ----------

    Vector operator*(const float scalar, const Vector &v) {
        return v * scalar;
    }

    Vector operator+(const float scalar, const Vector &v) {
        return v + scalar;
    }
}