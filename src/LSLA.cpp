#include "LSLA.h"

#include <algorithm>
#include <cmath>
#include <exception>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

namespace LSLA {
    namespace {
        constexpr int parallelThreshold = 32 * 1024;

        template <typename Function>
        void forEachRange(const int count, Function function) {
            // If there are no elements to process, we can return early.
            if (count == 0) {
                return;
            }
            // If the number of elements is below the threshold, we can process them in a single thread.
            if (count < parallelThreshold) {
                function(0, count, 0);
                return;
            }

            // If the number of elements is above the threshold, we can process them in parallel.
            const unsigned int availableWorkers = std::thread::hardware_concurrency(); // Detects # of available hardware threads on the system. 0 if not well-defined or not computable.
            // Use the minimum of the number of available workers and the number of elements to process.
            const int workerCount = std::min(
                count,
                static_cast<int>(availableWorkers == 0 ? 1 : availableWorkers));

            // If there is only one worker, we process the elements in a single thread.
            if (workerCount == 1) {
                function(0, count, 0);
                return;
            }
            // If there are multiple workers, we divide the elements into ranges and process them in parallel.
            const int elementsPerWorker = count / workerCount;
            // The remainder is the number of elements that cannot be evenly divided among the workers.
            // Distribute these elements among the first few workers.
            const int remainder = count % workerCount;
            std::vector<std::thread> workers;
            workers.reserve(workerCount); // Preallocate memory for the worker threads to avoid reallocations during the loop.
            std::exception_ptr workerException;
            std::mutex exceptionMutex;
            int begin = 0;

            try {
                for (int worker = 0; worker < workerCount; ++worker) {
                    const int rangeSize = elementsPerWorker + (worker < remainder ? 1 : 0);
                    const int end = begin + rangeSize;
                    workers.emplace_back([begin, end, worker, &function, &workerException, &exceptionMutex] {
                        try {
                            function(begin, end, worker);
                        } catch (...) {
                            std::lock_guard<std::mutex> lock(exceptionMutex);
                            if (!workerException) {
                                workerException = std::current_exception();
                            }
                        }
                    });
                    begin = end;
                }
            } catch (...) {
                for (std::thread &worker : workers) {
                    worker.join();
                }
                throw;
            }

            for (std::thread &worker : workers) {
                worker.join();
            }

            if (workerException) {
                std::rethrow_exception(workerException);
            }
        }
    }

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
        forEachRange(size, [this, &other](const int begin, const int end, int) {
            std::copy(other.data + begin, other.data + end, data + begin);
        });
    }

    Vector &Vector::operator=(const Vector &other) {
        if (this == &other) {
            return *this;
        }
        float *newData = other.size > 0 ? new float[other.size] : nullptr;
        forEachRange(other.size, [&other, newData](const int begin, const int end, int) {
            std::copy(other.data + begin, other.data + end, newData + begin);
        });
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
            throw std::invalid_argument("Vector size does not match other vector's size");
        }
        forEachRange(size, [this, &other](const int begin, const int end, int) {
            for (int i = begin; i < end; ++i) {
                data[i] += other.data[i];
            }
        });
        return *this;
    }

    Vector Vector::operator-(const Vector &other) const {
        Vector result(*this);
        result -= other;
        return result;
    }

    Vector &Vector::operator-=(const Vector &other) {
        if (other.size != size) {
            throw std::invalid_argument("Vector size does not match other vector's size");
        }
        forEachRange(size, [this, &other](const int begin, const int end, int) {
            for (int i = begin; i < end; ++i) {
                data[i] -= other.data[i];
            }
        });
        return *this;
    }

    float Vector::operator*(const Vector &other) const {
        if (other.size != size) {
            throw std::invalid_argument("Vector size does not match other vector's size");
        }
        if (size < parallelThreshold || std::thread::hardware_concurrency() <= 1) {
            float product = 0.0f;
            for (int i = 0; i < size; ++i) {
                product += data[i] * other.data[i];
            }
            return product;
        }

        const int workerCount = std::min(size, static_cast<int>(std::thread::hardware_concurrency()));
        std::vector<float> partialSums(workerCount, 0.0f);
        forEachRange(size, [this, &other, &partialSums](
                               const int begin, const int end, const int worker) {
            float partialSum = 0.0f;
            for (int i = begin; i < end; ++i) {
                partialSum += data[i] * other.data[i];
            }
            partialSums[worker] = partialSum;
        });

        float product = 0.0f;
        for (const float partialSum : partialSums) {
            product += partialSum;
        }
        return product;
    }

    Vector Vector::operator^(const Vector &other) const {
        if (other.size != size) {
            throw std::invalid_argument("Vector size does not match other vector's size");
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
        forEachRange(size, [this, scalar](const int begin, const int end, int) {
            for (int i = begin; i < end; ++i) {
                data[i] *= scalar;
            }
        });
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
        forEachRange(size, [this, scalar](const int begin, const int end, int) {
            for (int i = begin; i < end; ++i) {
                data[i] /= scalar;
            }
        });
        return *this;
    }

    Vector Vector::operator+(const float scalar) const {
        Vector result(*this);
        result += scalar;
        return result;
    }

    Vector &Vector::operator+=(const float scalar) {
        forEachRange(size, [this, scalar](const int begin, const int end, int) {
            for (int i = begin; i < end; ++i) {
                data[i] += scalar;
            }
        });
        return *this;
    }

    Vector Vector::operator-(const float scalar) const {
        Vector result(*this);
        result -= scalar;
        return result;
    }

    Vector &Vector::operator-=(const float scalar) {
        forEachRange(size, [this, scalar](const int begin, const int end, int) {
            for (int i = begin; i < end; ++i) {
                data[i] -= scalar;
            }
        });
        return *this;
    }

    Vector Vector::power(const float scalar) const {
        Vector result(size);
        forEachRange(size, [this, &result, scalar](const int begin, const int end, int) {
            for (int i = begin; i < end; ++i) {
                result.data[i] = std::pow(data[i], scalar);
            }
        });
        return result;
    }

    // ---------- Unary and comparison ----------
    Vector Vector::operator-() const {
        Vector result(size);
        forEachRange(size, [this, &result](const int begin, const int end, int) {
            for (int i = begin; i < end; ++i) {
                result.data[i] = -data[i];
            }
        });
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