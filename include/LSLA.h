#pragma once

namespace LSLA {
    // Add your declarations here
    void HelloWorld();

    class Vector{
        private:
            int size;
            float* data;

        public:
            Vector(int size);
            ~Vector();
            float& operator[](int index);

            ////// Vector to Vector operations ////////
            // Addition (Vector + Vector)
            Vector& operator+(const Vector& other);
            // Subtraction (Vector - Vector)
            Vector& operator-(const Vector& other);
            //Vector Cross Product (Vector x Vector)
            Vector& operator^(const Vector& other);
            // Dot Product (Vector . Vector)
            float operator*(const Vector& other);

            ////// Vector to Scalar operations ////////
            // Scalar multiplication (Vector * scalar)
            Vector& operator*(float scalar);
            // Scalar division (Vector / scalar)
            Vector& operator/(float scalar);
            // Scalar addition (Vector + scalar)
            Vector& operator+(float scalar);
            // Scalar subtraction (Vector - scalar)
            Vector& operator-(float scalar);
            // Scalar power (Vector ^ scalar)
            Vector& operator^(float scalar);
    };
}
