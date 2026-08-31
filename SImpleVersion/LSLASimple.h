#pragma once

#include <initializer_list>

namespace LSLA {
    void HelloWorld();

    class Vector {
    private:
        int size;
        float *data;

    public:
        // ---------- Construction / destruction ----------
        // Rule of five
        Vector();
        explicit Vector(int size);
        Vector(std::initializer_list<float> values);

        Vector(const Vector &other);
        Vector& operator=(const Vector &other);

        Vector(Vector &&other) noexcept;
        Vector& operator=(Vector &&other) noexcept;

        ~Vector();

        // ---------- Element access ----------
        float& operator[](int index);
        const float& operator[](int index) const;

        Vector& operator=(std::initializer_list<float> values);

        [[nodiscard]] int getSize() const;

        // ---------- Vector <-> Vector ----------
        Vector operator+(const Vector &other) const;
        Vector& operator+=(const Vector &other);

        Vector operator-(const Vector &other) const;
        Vector& operator-=(const Vector &other);

        // Dot product. Returns a scalar, so no reference question arises.
        float operator*(const Vector &other) const;

        // Cross product (defined for size == 3).
        Vector operator^(const Vector &other) const;

        // ---------- Vector <-> scalar ----------
        Vector operator*(float scalar) const;
        Vector& operator*=(float scalar);

        Vector operator/(float scalar) const;
        Vector& operator/=(float scalar);

        Vector operator+(float scalar) const;
        Vector& operator+=(float scalar);

        Vector operator-(float scalar) const;
        Vector& operator-=(float scalar);

        [[nodiscard]] Vector power(float scalar) const;

        // Unary negation: -v
        Vector operator-() const;

        bool operator==(const Vector &other) const;
        bool operator!=(const Vector &other) const;
    };

    // Free functions so the scalar can sit on the left: 2.0f * v, 2.0f + v.
    Vector operator*(float scalar, const Vector &v);
    Vector operator+(float scalar, const Vector &v);
}