#include "LSLA.h"
#include "LSLASimple.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace {
    constexpr int largeSize = 100000;

    void require(const bool condition, const char *message) {
        if (!condition) {
            throw std::runtime_error(message);
        }
    }

    template <typename Exception, typename Function>
    void requireThrows(Function function, const char *message) {
        try {
            function();
        } catch (const Exception &) {
            return;
        }
        throw std::runtime_error(message);
    }

    void testErrors() {
        requireThrows<std::invalid_argument>([] { LSLA::Vector vector(-1); },
                                            "negative vector sizes must fail");
        LSLA::Vector vector(2);
        requireThrows<std::out_of_range>([&vector] { static_cast<void>(vector[2]); },
                                         "out-of-range index must fail");
        requireThrows<std::invalid_argument>([&vector] { vector /= 0.0f; },
                                            "division by zero must fail");
        requireThrows<std::invalid_argument>(
            [&vector] { static_cast<void>(vector + LSLA::Vector(3)); },
            "mismatched vector sizes must fail");
    }

    void testBasicArithmetic() {
        LSLA::Vector left{1.0f, 2.0f, 3.0f};
        LSLA::Vector right{4.0f, 5.0f, 6.0f};
        const LSLA::Vector sum = left + right;
        const LSLA::Vector cross = left ^ right;

        require(sum[0] == 5.0f && sum[2] == 9.0f, "vector addition is incorrect");
        require(left * right == 32.0f, "dot product is incorrect");
        require(cross[0] == -3.0f && cross[1] == 6.0f && cross[2] == -3.0f,
                "cross product is incorrect");
        require((left * 2.0f)[2] == 6.0f, "scalar multiplication is incorrect");
        require((left.power(2.0f))[1] == 4.0f, "power is incorrect");
    }

    void testLargeParity() {
        LSLA::Vector optimizedLeft(largeSize);
        LSLA::Vector optimizedRight(largeSize);
        LSLASimple::Vector simpleLeft(largeSize);
        LSLASimple::Vector simpleRight(largeSize);

        for (int i = 0; i < largeSize; ++i) {
            const float left = static_cast<float>((i % 97) - 48) / 8.0f;
            const float right = static_cast<float>((i % 71) - 35) / 7.0f;
            optimizedLeft[i] = simpleLeft[i] = left;
            optimizedRight[i] = simpleRight[i] = right;
        }

        const LSLA::Vector optimizedResult =
            -(optimizedLeft + optimizedRight) * 1.5f + 2.0f;
        const LSLASimple::Vector simpleResult =
            -(simpleLeft + simpleRight) * 1.5f + 2.0f;

        for (int i = 0; i < largeSize; ++i) {
            require(optimizedResult[i] == simpleResult[i],
                    "parallel element-wise operation differs from reference");
        }

        const float optimizedDot = optimizedLeft * optimizedRight;
        const float simpleDot = simpleLeft * simpleRight;
        const float tolerance = std::max(1.0f, std::abs(simpleDot)) * 1.0e-4f;
        require(std::abs(optimizedDot - simpleDot) <= tolerance,
                "parallel dot product exceeds rounding tolerance");
    }
}

int main() {
    try {
        testErrors();
        testBasicArithmetic();
        testLargeParity();
    } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
    return 0;
}
