#pragma once

#include <initializer_list>

namespace LSLA {
    // Add your declarations here
    void HelloWorld();

    class Vector {
    private:
        int size;
        float *data;

    public:
        Vector();
        explicit Vector(int size);

        Vector(const Vector &other);
        Vector &operator=(const Vector &other);

        ~Vector();

        float &operator[](int index) const;

        Vector &operator=(std::initializer_list<float> values);

        void setData(std::initializer_list<float> values) const;

        [[nodiscard]] int getSize() const;

        ////// Vector to Vector operations ////////
        // Addition (Vector + Vector)
        Vector &operator+(const Vector &other);
        Vector &operator+=(const Vector &other);

        // Subtraction (Vector - Vector)
        Vector &operator-(const Vector &other);
        Vector &operator-=(const Vector &other);

        //Vector Cross Product (Vector x Vector)
        Vector operator^(const Vector &other) const;

        // Dot Product (Vector . Vector)
        float operator*(const Vector &other) const;

        ////// Vector to Scalar operations ////////
        // Scalar multiplication (Vector * scalar)
        Vector &operator*(float scalar);

        // Scalar division (Vector / scalar)
        Vector &operator/(float scalar);

        // Scalar addition (Vector + scalar)
        Vector &operator+(float scalar);

        // Scalar subtraction (Vector - scalar)
        Vector &operator-(float scalar);

        // Scalar power (Vector ^ scalar)
        Vector &operator^(float scalar);
    };
}
